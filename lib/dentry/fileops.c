/*
 *	standard GIO implementation doesn't directly support
 *	traversing the filesystem hierachy. 
 *	so we need to implement one.
 */
#include <glib/gstdio.h>

#include "fileops.h"
#include "fileops_error_reporting.h"


static gboolean _dummy_func		(GFile* file, gpointer data);

static gboolean _delete_files_async	(GFile* file, gpointer data);
static gboolean _trash_files_async	(GFile* file, gpointer data);
static gboolean _move_files_async	(GFile* file, gpointer data);
static gboolean _copy_files_async	(GFile* file, gpointer data);


/*
 *	@dir	: file or directory to traverse 
 *	@pre_hook: pre-processing function, this used in move and copy
 *	@post_hook: post-processing function, this used in delete and trash.
 *	@data	: data passed to callback function.
 *	          currently we only use this as GFile* which is the fileops destination
 *	          corresponding to @dir. for each recursive level, we should update 
 *	          data to ensure that @dir and @data are consistent.
 *
 *	NOTE: 1.if dir is a file, applying callback and return.
 *	        if dir is a directory, traversing the directory tree 
 *	      2.we don't follow symbol links.
 *   	      3.there's a race condition in checking @dir type before 
 *		enumerating @dir. so we don't check @dir type. 
 *		if @dir is a file, we handle it in G_IO_ERROR_NOT_DIRECTORY.
 *	      4. (move, copy) and (delete, trash) behave differently.
 *	         (move, copy) first create the directory then create files in the directory
 *	         (delete, trash) first delete files in the directory then delete the directory
 *	         so we need a pre_hook and post_hook separately.
 *	       
 *
 *	TODO: change "standard::*" to the attributes we actually needed.
 */
//data used by Traverse Directory (TD)
typedef struct _TDData TDData;
struct _TDData
{
    GFile*	 dest_file;
    GCancellable* cancellable;
};
//deep copy
static TDData* new_td_data ()
{
    TDData* new_tddata = NULL;
    new_tddata = g_malloc0 (sizeof (TDData));
    return new_tddata;
}
static void* free_td_data (TDData* tddata)
{
    g_object_unref (tddata->dest_file);
    g_free (tddata);
}

// src ---> data->dest
// we make use of 'goto' to minimize duplicated 'g_free*' statement
gboolean
traverse_directory (GFile* src, GFileProcessingFunc pre_hook, GFileProcessingFunc post_hook, gpointer data)
{
    gboolean retval = TRUE;

    GError* error = NULL;
    GFileEnumerator* src_enumerator = NULL;

    src_enumerator = g_file_enumerate_children (src, 
					       "standard::*",
					       G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
					       NULL,
					       &error);
    if (error != NULL)
    {
	//src_enumerator must be NULL, nothing to free.
	switch (error->code)
	{
	    case G_IO_ERROR_NOT_FOUND:
		//TODO: showup a message box and quit.
		break;
	    case G_IO_ERROR_NOT_DIRECTORY:
		//TODO:we're using a file. 
		if (pre_hook (src, data) == FALSE ||
		    post_hook (src, data) == FALSE)
		{
		    g_error_free (error);
		    return FALSE;
		}
		else
		{
		    g_error_free (error);
		    return TRUE;
		}
	    default:
		break;
	}
	g_warning ("traverse_directory 1: %s", error->message);
	g_error_free (error);

	return TRUE;
    }

    //here, we must be in a directory.
    //check if it's a symbolic link
    if (pre_hook (src, data) == FALSE) //src_enumerator must be freed
    {
	retval = FALSE;
	goto post_processing;
    }
#if 1    
    char* src_uri = NULL;
    src_uri = g_file_get_uri (src);
    g_debug ("traverse_directory: chdir to : %s", src_uri);
#endif

    GFileInfo* file_info = NULL;
    while ((file_info = g_file_enumerator_next_file (src_enumerator, NULL, &error)) != NULL)
    {
	//this should not be freed with g_free(). it'll be freed when we call g_object_unref
	//on file_info
#if 1
	const char* src_child_name = g_file_info_get_name (file_info);
	g_debug ("traverse_directory: %s", src_child_name);
#endif 

	TDData* tddata = NULL;
	GFile* src_child_file = NULL; 
	GFile* dest_dir = NULL;   //corresponding to src 
	GFile* dest_child_file = NULL;  //corresponding to src_child_file.
	    
	tddata = new_td_data ();
	src_child_file = g_file_get_child (src, src_child_name);
	dest_dir = ((TDData*)data)->dest_file;

	if (dest_dir != NULL)
	    dest_child_file = g_file_get_child (dest_dir, src_child_name);
#if 1
	char* dest_child_file_uri = g_file_get_uri (dest_child_file);
	g_debug ("dest_child_file_uri: %s", dest_child_file_uri);
	g_free (dest_child_file_uri);
#endif

	tddata->dest_file = dest_child_file;
	tddata->cancellable = ((TDData*)data)->cancellable;
	//TODO:
	//get out the loop recursively when operation is cancelled.
	retval = traverse_directory (src_child_file, pre_hook, post_hook, tddata);

	g_object_unref (src_child_file);
	free_td_data (tddata);

	g_object_unref (file_info);
	file_info = NULL;

	if (retval == FALSE)
	    goto post_processing;
    }
    //checking errors
    if (error != NULL)
    {
	g_warning ("traverse_directory 2: %s", error->message);
	g_error_free (error);
    }

#if 1
    //change to parent directory.
    g_debug ("traverse_directory: come out: %s", src_uri);
    g_free (src_uri);
#endif  

post_processing:
    //close enumerator.
    g_file_enumerator_close (src_enumerator, NULL, &error);
    g_object_unref (src_enumerator);
    //checking errors
    if (error != NULL)
    {
	g_warning ("traverse_directory 3: %s", error->message);
	g_error_free (error);
    }

    //after processing child node. processing this directory.
    if (post_hook (src, data) == FALSE)
	return FALSE;

    return retval;
}

/*
 *	@file_list : files(or directories) to delete.
 *	@num	   : number of files(or directories) in file_list
 *	pre_hook =NULL
 *	post_hook = _delete_files_async
 */
void
fileops_delete (GFile* file_list[], guint num)
{
    g_debug ("fileops_delete: Begin deleting files");
    GCancellable* delete_cancellable = g_cancellable_new ();
    TDData* data = g_malloc0 (sizeof (TDData));
    int i;
    for (i = 0; i < num; i++)
    {
	GFile* src = file_list[i];
#if 1
	char* src_uri = g_file_get_uri (src);
	g_debug ("fileops_delete: file %d: %s", i, src_uri);
	g_free (src_uri);
#endif
	data->dest_file = NULL;
	data->cancellable = delete_cancellable;

	traverse_directory (src, _dummy_func, _delete_files_async, data);
    }
    g_object_unref (data->cancellable);
    g_free (data);
    g_debug ("fileops_delete: End deleting files");
}
/*
 *	@file_list : files(or directories) to trash.
 *	@num	   : number of files(or directories) in file_list
 *	NOTE: trashing is special because we don't need to 
 *	      traverse_directory. the default implementation can 
 *	      recursively trash files.
 */
void
fileops_trash (GFile* file_list[], guint num)
{
    g_debug ("fileops_trash: Begin trashing files");
    GCancellable* trash_cancellable = g_cancellable_new ();
    TDData* data = g_malloc0 (sizeof (TDData));
    int i;
    for (i = 0; i < num; i++)
    {
	GFile* src = file_list[i];
#if 1
	char* src_uri = g_file_get_uri (src);
	g_debug ("fileops_trash: file %d: %s", i, src_uri);
	g_free (src_uri);
#endif
	data->dest_file = NULL;
	data->cancellable = trash_cancellable;

	_trash_files_async (src, data);
	//traverse_directory (dir, _dummy_func, _trash_files_async, NULL);
    }
    g_object_unref (data->cancellable);
    g_free (data);
    g_debug ("fileops_trash: End trashing files");
}
/*
 *	@file_list : files(or directories) to move.
 *	@num	   : number of files(or directories) in file_list
 *	@dest	   : destination directory.
 *
 *	NOTE: moving is special because we don't need to 
 *	      traverse_directory. the default implementation can 
 *	      recursively trash files.
 */
void
fileops_move (GFile* file_list[], guint num, GFile* dest_dir)
{
    g_debug ("fileops_move: Begin moving files");
    GCancellable* move_cancellable = g_cancellable_new ();
    TDData* data = g_malloc0 (sizeof (TDData));
    int i;
    for (i = 0; i < num; i++)
    {
	GFile* src = file_list[i];
#if 1
	char* src_uri = g_file_get_uri (src);
	char* dest_dir_uri = g_file_get_uri (dest_dir);
	g_debug ("fileops_move: file %d: %s to dest: %s", i, src_uri, dest_dir_uri);
	g_free (src_uri);
	g_free (dest_dir_uri);
#endif
	//make sure dest_dir is a directory before proceeding.
	GFileType type = g_file_query_file_type (dest_dir, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL);
	if (type != G_FILE_TYPE_DIRECTORY)
	{
	    //TODO: symbolic links
	    return;
	}
	char* src_basename= g_file_get_basename (src);
	GFile* move_dest_file = g_file_get_child (dest_dir, src_basename);
	g_free (src_basename);

	data->dest_file = move_dest_file;

	_move_files_async (src, data);
	//traverse_directory (dir, _move_files_async, _dummy_func, move_dest_gfile);
	g_object_unref (move_dest_file);
    }
    g_object_unref (data->cancellable);
    g_free (data);
    g_debug ("fileops_move: End moving files");
}
/*
 *	@file_list : files(or directories) to trash.
 *	@num	   : number of files(or directories) in file_list
 *	pre_hook = _copy_files_async
 *	post_hook = NULL
 */
void
fileops_copy (GFile* file_list[], guint num, GFile* dest_dir)
{
    g_debug ("fileops_copy: Begin copying files");
    GCancellable* copy_cancellable = g_cancellable_new ();
    TDData* data = g_malloc0 (sizeof (TDData));
    int i;
    for (i = 0; i < num; i++) 
    {
        GFile* src = file_list[i];
#if 1
        char* src_uri= g_file_get_uri (src);
        char* dest_dir_uri = g_file_get_uri (dest_dir);
        g_debug ("fileops_copy: file %d: %s to dest_dir: %s", i, src_uri, dest_dir_uri);
        g_free (src_uri);
        g_free (dest_dir_uri);
#endif

        //make sure dest_dir is a directory before proceeding.
        GFileType type = g_file_query_file_type (dest_dir, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL);
        if (type != G_FILE_TYPE_DIRECTORY)
        {
	    //TODO: how to handle symbolic links
            return;
        }

        char* src_basename = g_file_get_basename (src);
        GFile* copy_dest_file = g_file_get_child (dest_dir, src_basename);
        g_free (src_basename);

	data->dest_file = copy_dest_file;

        traverse_directory (src, _copy_files_async, _dummy_func, data);

        g_object_unref (copy_dest_file);
    }
    g_object_unref (data->cancellable);
    g_free (data);
    g_debug ("fileops_copy: End copying files");
}
// internal functions 
// TODO : setup a dialog, support Cancelling and show progress bar.
//
// hook function return value:
// TRUE: continue operation 
//       CONFLICT_RESPONSE_SKIP
//       CONFLICT_RESPONSE_RENAME
//	 CONFLICT_RESPONSE_REPLACE
// FALSE: get out of traverse_directory.
//        GTK_RESPONSE_CANCEL

static gboolean 
_dummy_func (GFile* file, gpointer data)
{
    return TRUE;
}

static gboolean
_delete_files_async (GFile* file, gpointer data)
{
    gboolean retval = TRUE;

    TDData* _data = (TDData*) data;

    GError* error = NULL;
    GCancellable* _delete_cancellable = NULL;

    _delete_cancellable = _data->cancellable;
    g_file_delete (file, _delete_cancellable, &error);

    if (error != NULL)
    {
	//show error dialog
	g_cancellable_cancel (_delete_cancellable);
	g_warning ("_delete_files_async: %s", error->message);
	//fileops_delete_trash_error_show_dialog ("delete", error, file, NULL);
	g_error_free (error);
    }
#if 1
    char* file_uri = g_file_get_uri (file);
    g_debug ("_delete_files_async: delete : %s", file_uri);
    g_free (file_uri);
#endif

    return retval;
}

static gboolean
_trash_files_async (GFile* file, gpointer data)
{
    gboolean retval = TRUE;

    TDData* _data = (TDData*) data;

    GError* error = NULL;
    GCancellable* _trash_cancellable = NULL;

    _trash_cancellable = _data->cancellable;
    g_file_trash (file, _trash_cancellable, &error);

    if (error != NULL)
    {
	g_cancellable_cancel (_trash_cancellable);
	g_warning ("_trash_files_async: %s", error->message);
	g_error_free (error);
    }
#if 1
    char* file_uri = g_file_get_uri (file);
    g_debug ("_trash_files_async: trash : %s", file_uri);
    g_free (file_uri);
#endif

    return retval;
}
/*
 */
static gboolean
_move_files_async (GFile* src, gpointer data)
{
    gboolean retval;

    TDData* _data = (TDData*) data;

    GError* error = NULL;
    GCancellable* _move_cancellable = NULL;
    GFile* dest = NULL;

    _move_cancellable = _data->cancellable;
    dest = _data->dest_file;
    g_file_move (src, dest,
	         G_FILE_COPY_NOFOLLOW_SYMLINKS,
		 _move_cancellable,
		 NULL,
		 NULL,
		 &error);
    if (error != NULL)
    {
	g_cancellable_cancel (_move_cancellable);
	g_warning ("_move_files_async: %s", error->message);
	//TEST:
	FileOpsResponse* response;
	response = fileops_move_copy_error_show_dialog ("copy", error, src, dest, NULL);

	switch (response->response_id)
	{
	    case GTK_RESPONSE_CANCEL:
		//cancel all operations
		g_debug ("response : Cancel");
		retval = FALSE;
		break;
	    
	    case CONFLICT_RESPONSE_SKIP:
	        //skip, imediately return.
	        g_debug ("response : Skip");
		retval = TRUE;
	        break;
	    case CONFLICT_RESPONSE_RENAME:
		//rename, redo operations
		g_debug ("response : Rename");
		retval = TRUE;
	        break;
	    case CONFLICT_RESPONSE_REPLACE:
	        //first delete and redo operations
	        g_debug ("response : Replace");
		retval = TRUE;
	        break;
	    default:
		retval = FALSE;
	        break;
	}

	g_error_free (error);
    }
#if 1
    char* src_uri = g_file_get_uri (src);
    char* dest_uri = g_file_get_uri (dest);
    g_debug ("_move_files_async: move %s to %s", src_uri, dest_uri);
    g_free (src_uri);
    g_free (dest_uri);
#endif

    return retval;
}
/*
 *  
 */
static gboolean
_copy_files_async (GFile* src, gpointer data)
{
    gboolean retval;

    TDData* _data = (TDData*) data;

    GError* error = NULL;
    GCancellable* _copy_cancellable = NULL;
    GFile* dest = NULL;

    _copy_cancellable = _data->cancellable;
    dest = _data->dest_file;
    
    //because @dest doesn't exist, we should check @src instead.
    GFileType type = g_file_query_file_type (src, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL);
    if (type == G_FILE_TYPE_DIRECTORY)
    {
	//TODO: change permissions
	g_file_make_directory (dest, NULL, &error);
	if (error != NULL)
	{
	    g_debug ("_copy_files_async: %s", error->message);
	}
	char* dest_uri = g_file_get_uri (dest);
	g_debug ("_copy_files_async: mkdir : %s", dest_uri);
	g_free (dest_uri);
    }
    else
    {
	g_file_copy (src, dest,
		     G_FILE_COPY_NOFOLLOW_SYMLINKS,
		     _copy_cancellable,
		     NULL,
		     NULL,
		     &error);
	if (error != NULL)
	{
	    g_cancellable_cancel (_copy_cancellable);
	    g_warning ("_copy_files_async: %s", error->message);
	    //TEST:
	    FileOpsResponse* response;
	    response = fileops_move_copy_error_show_dialog ("copy", error, src, dest, NULL);

	    switch (response->response_id)
	    {
		case GTK_RESPONSE_CANCEL:
		    //cancel all operations
		    g_debug ("response : Cancel");
		    retval = FALSE;
		    break;

		case CONFLICT_RESPONSE_SKIP:
	            //skip, imediately return.
	            g_debug ("response : Skip");
		    retval = TRUE;
	            break;
		case CONFLICT_RESPONSE_RENAME:
		    //rename, redo operations
		    g_debug ("response : Rename");
		    retval = TRUE;
	            break;
	        case CONFLICT_RESPONSE_REPLACE:
	            //first delete and redo operations
	            g_debug ("response : Replace");
		    retval = TRUE;
	            break;
	        default:
		    retval = FALSE;
	            break;
	    }

	    g_error_free (error);
	}
#if 1
	char* src_uri = g_file_get_uri (src);
	char* dest_uri = g_file_get_uri (dest);
	g_debug ("_copy_files_async: copy %s to %s", src_uri, dest_uri);
	g_free (src_uri);
	g_free (dest_uri);
#endif
    }

    return retval;
}
