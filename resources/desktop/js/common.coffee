#Copyright (c) 2011 ~ 2012 Deepin, Inc.
#              2011 ~ 2012 snyh
#
#Author:      Cole <phcourage@gmail.com>
#Maintainer:  Cole <phcourage@gmail.com>
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, see <http://www.gnu.org/licenses/>.

# const strings and functions for desktop internal DND operation
_DND_DATA_TYPE_NAME_ = "text/operate-type"
_DND_DESKTOP_MARK_ = "desktop_internal"

_SET_DND_INTERNAL_FLAG_ = (evt) ->
    evt.dataTransfer.setData(_DND_DATA_TYPE_NAME_, _DND_DESKTOP_MARK_)


_IS_DND_INTERLNAL_ = (evt) ->
    evt.dataTransfer.getData(_DND_DATA_TYPE_NAME_) == _DND_DESKTOP_MARK_


# item real size on grid
_ITEM_WIDTH_ = 80 + 6 * 2
_ITEM_HEIGHT_ = 84 + 4 * 2


# delay interval time before renaming item
_RENAME_TIME_DELAY_ = 600


# id string for "computer" item
_ITEM_ID_COMPUTER_  = "Computer_Virtual_Dir"
# id string for "home" item
_ITEM_ID_USER_HOME_ = "Home_Virtual_Dir"
# id string for "trash bin" item
_ITEM_ID_TRASH_BIN_ = "Trash_Virtual_Dir"
# id string for "Deepin Software Center" item
_ITEM_ID_DSC_ = "DSC_Virtual_Button"

# id string for "computer" icon
_ICON_ID_COMPUTER_  = "computer"
# id string for "home" icon
_ICON_ID_USER_HOME_ = "deepin-user-home"
# id string for "trash bin" normal icon
_ICON_ID_TRASH_BIN_FULL_ = "user-trash-full"
# id string for "trash bin" empty icon
_ICON_ID_TRASH_BIN_ = "user-trash"
# id string for "Deepin Software Center" icon
_ICON_ID_DSC_ = "deepin-software-center"


# icon name for file attributes
_FAI_READ_ONLY_  = "emblem-readonly"
_FAT_SYM_LINK_   = "emblem-symbolic-link"
_FAT_UNREADABLE_ = "emblem-unreadable"

# store the entry for desktop
g_desktop_entry = DCore.DEntry.create_by_path(DCore.Desktop.get_desktop_path())


# const names to get configs
_CFG_SHOW_COMPUTER_ICON_ = "show-computer-icon"
_CFG_SHOW_HOME_ICON_ = "show-home-icon"
_CFG_SHOW_TRASH_BIN_ICON_ = "show-trash-icon"
_CFG_SHOW_DSC_ICON_ = "show-dsc-icon"

# wrapper func to get configs
_GET_CFG_BOOL_ = (val) ->
    DCore.Desktop.get_config_boolean(val)


# DBus handler for invoke nautilus filemanager
try
    g_dbus_nautilus = DCore.DBus.session("org.freedesktop.FileManager1")
catch e
    echo "error when init nautilus DBus interface(#{e})"
    g_dbus_nautilus = null
