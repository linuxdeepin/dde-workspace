//get a list of GVolumes
#include <gio/gio.h>

static GList *	_get_trash_dirs_for_mount	(GMount *mount);
static gboolean _empty_trash_job		(GIOSchedulerJob *io_job,
						 GCancellable* cancellable,
						 gpointer user_data);
static gboolean _empty_trash_job_done		(gpointer user_data);
static void	_delete_trash_file		(GFile *file,
						 gboolean del_file,
						 gboolean del_children);

void desktop_empty_trash ()
{
    GList* trash_list = NULL;

    GVolumeMonitor* vol_monitor = g_volume_monitor_get ();
    GList* mount_list = g_volume_monitor_get_mounts (vol_monitor);
    g_object_unref (vol_monitor);

    //iterate through all mounts
    GList* l;
    for (l = mount_list; l != NULL; l = l->next) 
    {
	trash_list = g_list_concat (trash_list, 
		                    _get_trash_dirs_for_mount (l->data));
    }
    g_list_free_full (mount_list, g_object_unref);
    //add 'trash:' prefix
    trash_list = g_list_prepend (trash_list,
				 g_file_new_for_uri ("trash:"));

    g_io_scheduler_push_job (_empty_trash_job,
			     trash_list,
			     NULL,
			     0,
			     NULL);
}
static GList *
_get_trash_dirs_for_mount (GMount *mount)
{
    GFile *root;
    GFile *trash;
    char *relpath;
    GList *list;

    root = g_mount_get_root (mount);
    if (root == NULL) 
	return NULL;

    list = NULL;
    if (g_file_is_native (root))
    {
	relpath = g_strdup_printf (".Trash/%d", getuid ());
	trash = g_file_resolve_relative_path (root, relpath);
	g_free (relpath);

	list = g_list_prepend (list, g_file_get_child (trash, "files"));
	list = g_list_prepend (list, g_file_get_child (trash, "info"));
		
	g_object_unref (trash);
		
	relpath = g_strdup_printf (".Trash-%d", getuid ());
	trash = g_file_get_child (root, relpath);
	g_free (relpath);

	list = g_list_prepend (list, g_file_get_child (trash, "files"));
	list = g_list_prepend (list, g_file_get_child (trash, "info"));
		
	g_object_unref (trash);
    }
    g_object_unref (root);

    return list;
}

static gboolean
_empty_trash_job (GIOSchedulerJob *io_job,
	          GCancellable* cancellable,
		  gpointer user_data)
{
    GList* trash_list = (GList*) user_data;

    GList* l;
    for (l = trash_list; l != NULL; l = l->next) 
	    _delete_trash_file (l->data, FALSE, TRUE);

    g_io_scheduler_job_send_to_mainloop_async (io_job,
	                                       _empty_trash_job_done,
					       user_data,
					       NULL);
}
static gboolean
_empty_trash_job_done (gpointer user_data)
{
    g_list_free_full (user_data, g_object_unref);
}
static void
_delete_trash_file (GFile *file,
		   gboolean del_file,
		   gboolean del_children)
{
    GFileInfo *info;
    GFile *child;
    GFileEnumerator *enumerator;

    if (del_children)
    {
	enumerator = g_file_enumerate_children (file,
						G_FILE_ATTRIBUTE_STANDARD_NAME ","
						G_FILE_ATTRIBUTE_STANDARD_TYPE,
						G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
						NULL, NULL);
	if (enumerator) 
	{
	    while ((info = g_file_enumerator_next_file (enumerator, NULL, NULL)) != NULL)
	    {
		child = g_file_get_child (file, g_file_info_get_name (info));
		_delete_trash_file (child, TRUE,
				   g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY);
		g_object_unref (child);
		g_object_unref (info);
	    }
	    g_file_enumerator_close (enumerator, NULL, NULL);
	    g_object_unref (enumerator);
	}
    }
    if (del_file) 
    {
	g_file_delete (file, NULL, NULL);
    }
}
