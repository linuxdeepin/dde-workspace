class PluginHandle extends Widget
    constructor : (@parent_id) ->
        @id = "handle-#{@parent_id}"
        super(@id)
        @element.setAttribute("draggable", "true")
        @offset_pos = {x : -1, y : -1}


    do_mouseover : (evt) =>
        if not (w = Widget.look_up(@parent_id))? then return
        w.add_css_class("plugin_hover_border")
        return


    do_mouseout : (evt) =>
        if not (w = Widget.look_up(@parent_id))? then return
        w.remove_css_class("plugin_hover_border")
        return


    do_dragstart : (evt) =>
        evt.stopPropagation()

        _SET_DND_INTERNAL_FLAG_(evt)
        drag_pos = pixel_to_pos(evt.clientX, evt.clientY, 1, 1)
        @offset_pos.x = drag_pos.x
        @offset_pos.y = drag_pos.y
        if (w = Widget.look_up(@parent_id))? then w.add_css_class("plugin_DND_border")
        return


    do_dragend : (evt) =>
        evt.stopPropagation()
        if not (w = Widget.look_up(@parent_id))? then return
        old_pos = w.get_pos()
        new_pos = pixel_to_pos(evt.clientX, evt.clientY, old_pos.width, old_pos.height)
        new_pos.x -= (@offset_pos.x - old_pos.x)
        new_pos.x = 0 if new_pos.x < 0
        new_pos.y -= (@offset_pos.y - old_pos.y)
        new_pos.y = 0 if new_pos.y < 0
        if not detect_occupy(new_pos, @parent_id)
            move_to_somewhere(w, new_pos)
        w.remove_css_class("plugin_DND_border")
        return


class DesktopPluginItem extends Widget
    constructor: (@id)->
        super
        @_position = {x:-1, y:-1, width:1, height:1}
        widget_item.push(@id)
        attach_item_to_grid(@)
        @handle = new PluginHandle(@id)
        @element.appendChild(@handle.element)
        @container = document.createElement("div")
        @container.setAttribute("class", "PluginContainer")
        @element.appendChild(@container)


    get_id : =>
        @id


    get_pos : =>
        x : @_position.x
        y : @_position.y
        width : @_position.width
        height : @_position.height


    set_pos : (info) =>
        @_position.x = info.x
        @_position.y = info.y
        return


    set_size : (info) =>
        @_position.width = info.width
        @_position.height = info.height
        real_width = @_position.width * _ITEM_WIDTH_
        real_height = @_position.height * _ITEM_HEIGHT_
        @element.style.width = "#{real_width}px"
        @element.style.height = "#{real_height}px"
        real_height = real_height - @handle.element.offsetHeight
        @container.style.width = "#{real_width}px"
        @container.style.height = "#{real_height}px"
        return


    do_mousedown : (evt) ->
        evt.stopPropagation()
        return


    do_click : (evt) ->
        evt.stopPropagation()
        return


    do_mouseup : (evt) ->
        evt.stopPropagation()
        return


    do_rightclick : (evt) ->
        evt.stopPropagation()
        evt.preventDefault()
        return


    do_buildmenu : (evt) ->
        []


    do_keydown : (evt) ->
        evt.stopPropagation()
        return


    do_keypress : (evt) ->
        evt.stopPropagation()
        return


    do_keyup : (evt) ->
        evt.stopPropagation()
        return


    move: (x, y) =>
        style = @element.style
        style.position = "absolute"
        style.left = x
        style.top = y


class DesktopPlugin extends Plugin
    constructor: (@path, @name)->
        @item = new DesktopPluginItem(@name)
        super('desktop', @path, @name, @item.container)


    set_pos: (info)->
        move_to_somewhere(@item, info)


    destroy: ->
        @host.parentElement.removeChild(@host)
        delete_widget(@item)


    wrap_element: (child, width, height)->
        @host.appendChild(child)
        pos = @item.get_pos()
        pos.width = width
        pos.height = height
        @item.set_size(pos)


load_plugins = ->
    DCore.init_plugins('desktop')
    for p in DCore.get_plugins("desktop")
        new DesktopPlugin(get_path_base(p), get_path_name(p))
    return


find_free_position_for_widget = (info, id = null) ->
    new_pos = {x : 0, y : 0, width : info.width, height : info.height}
    x_pos = cols - 1
    while (x_pos = x_pos - info.width + 1) > -1
        new_pos.x = x_pos
        for i in [0 ... (rows - info.height)]
            new_pos.y = i
            if not detect_occupy(new_pos, id)
                return new_pos
    return null


place_all_widgets = ->
    for i in widget_item
        continue if not (w = Widget.look_up(i))?
        if not load_position(i)? and (new_pos = find_free_position_for_widget(w.get_pos(), w.get_id()))?
            echo "#{new_pos?.width}x#{new_pos?.height} in (#{new_pos?.x}, #{new_pos?.y})"
            move_to_somewhere(w, new_pos)
        else
            move_to_anywhere(w)
    return
