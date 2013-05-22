#Copyright (c) 2011 ~ 2012 Deepin, Inc.
#              2011 ~ 2012 bluth
#
#encoding: utf-8
#Author:      bluth <yuanchenglu@linuxdeepin.com>
#Maintainer:  bluth <yuanchenglu@linuxdeepin.com>
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

# use:(recommended)
#     cityid = 10101011
#     weatherdata = new WeatherData(cityid)
#     weatherdata_now = localStorage.getItem("weatherdata_now_storage")
#     weatherdata_more = localStorage.getItem("weatherdata_more_storage")
# or 
#     cityid = 10101011
#     weatherdata  = new WeatherData(cityid)
#     weatherdata_now = weatherdata.weatherdata_now
#     weatherdata_more = weatherdata.weatherdata_more

class WeatherData
    @weatherdata_now = null
    @weatherdata_more = null

    constructor: (cityid)->
        if cityid isnt null && cityid isnt ""
            localStorage.setItem("cityid_storage",cityid)
            @cityid = localStorage.getItem("cityid_storage")
            @url_nowweather_str = @url_nowweather(cityid)
            @url_moreweather_str = @url_moreweather(cityid)
        else return 0

    url_nowweather: (cityid)-> 
        return "http://www.weather.com.cn/data/sk/"+ cityid + ".html" 
    url_moreweather: (cityid)->
        return "http://m.weather.com.cn/data/" + cityid + ".html"

    ajax : (url, callback) ->
        xhr = new XMLHttpRequest()
        xhr.open("GET", url, true)
        xhr.send(null)
        xhr.onreadystatechange = ->
            if (xhr.readyState == 4 and xhr.status == 200)
                if xhr.responseText isnt ""
                    callback?(xhr)
                    echo "XMLHttpRequest receive all data."
            else if xhr.status is 404
                echo "XMLHttpRequest can't find the url ."
            else if xhr.status is 0
                echo "your computer are not connected to the Internet"
            return xhr.status  

    Get_weatherdata_now:(cityid = @cityid)->
        if cityid isnt "" && cityid isnt null
            # echo "Get_weatherdata_now"
            @ajax(@url_nowweather(cityid),(xhr)=>
                localStorage.setItem("weatherdata_now_storage",xhr.responseText)
                @weatherdata_now = localStorage.getObject("weatherdata_now_storage")
            )
        else return 0
    Get_weatherdata_more:(cityid = @cityid)->
        if cityid isnt "" && cityid isnt null
            # echo "Get_weatherdata_more"
            @ajax(@url_moreweather(cityid),(xhr)=>
                localStorage.setItem("weatherdata_more_storage",xhr.responseText)
                @weatherdata_more = localStorage.getObject("weatherdata_more_storage")
            )
        else return 0