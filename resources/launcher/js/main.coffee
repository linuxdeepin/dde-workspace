#Copyright (c) 2011 ~ 2012 Deepin, Inc.
#              2011 ~ 2012 snyh
#
#Author:      snyh <snyh@snyh.org>
#Maintainer:  snyh <snyh@snyh.org>
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

s_box.setAttribute("placeholder", _("Type to search..."))


do_workarea_changed = (alloc)->
    height = alloc.height
    document.body.style.maxHeight = "#{height}px"
    $('#grid').style.maxHeight = "#{height-60}px"
DCore.signal_connect('workarea_changed', do_workarea_changed)
DCore.signal_connect("lost_focus", (info)->
    if s_dock.LauncherShouldExit_sync(info.xid)
        DCore.Launcher.exit_gui()
)
DCore.Launcher.notify_workarea_size()


_select_timeout_id = 0
create_category = (info) ->
    el = document.createElement('div')
    el.setAttribute('class', 'category_name')
    el.setAttribute('cat_id', info.ID)
    el.innerHTML = "
    <div>#{info.Name}</div>
    "
    el.addEventListener('click', (e) ->
        e.stopPropagation()
        grid_load_category(info.ID)
    )
    el.addEventListener('mouseover', (e)->
        e.stopPropagation()
        grid_load_category(info.ID)
    )
    return el

append_to_category = (cat) ->
    $('#category').appendChild(cat)

append_to_category  create_category(
    "ID" : -1
    "Name": _("All")
)

$("body").addEventListener("click", (e)->
    e.stopPropagation()
    if e.target != $("#category")
        DCore.Launcher.exit_gui()
)

for info in DCore.Launcher.get_categories()
    c = create_category(info)
    append_to_category(c)

grid_load_category(-1) #the All applications' ID is zero.
