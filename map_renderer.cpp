#include "map_renderer.h"
#include <iostream>
#include <vector>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
namespace transport_catalogue {



void MapRenderer::RenderMap(std::map<std::string_view,std::vector<const domain::Stop *>>  bus_stops){
    std::vector<geo::Coordinates> geo_coords;
    geo_coords.reserve(bus_stops.size());
    for(auto const & bus:bus_stops){
        for(auto const &stop: bus.second){
            geo_coords.push_back(stop->coordinates);
        }
    }

    const SphereProjector proj{
                               geo_coords.begin(), geo_coords.end(),
                               render_settings_.width,
                               render_settings_.height,
                               render_settings_.padding };

    svg::Document doc;
    std::string_view name;
    int color_index = 0;    
    std::vector<std::string> color_palette = render_settings_.color_palette;
    VectorStringToRgb(color_palette);


    for(auto const & bus:bus_stops){
        svg::Polyline poly_line = svg::Polyline();
        poly_line.SetFillColor("none");
        poly_line.SetStrokeWidth(render_settings_.line_width);
        poly_line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        poly_line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        poly_line.SetStrokeColor(color_palette[color_index % color_palette.size()]);

        for(auto const &stop: bus.second){
            poly_line.AddPoint(proj(stop->coordinates));
        }

        if(bus.second.size()  >0 ) {
            doc.Add(poly_line);
            ++color_index;
        }
    }

    doc.Render(out_);
}

void MapRenderer::VectorStringToRgb(std::vector<Color> &color_palette){
    for(auto iter = color_palette.begin(); iter != color_palette.end();++iter){
        if(iter->find("[") != iter->npos){
            *iter = StringToRgb(*iter);
        }
    }
}

std::string MapRenderer::StringToRgb(std::string color_str){
    if(color_str.find("[") == color_str.npos) return color_str;
    int commas = std::count_if(color_str.begin(),color_str.end(),[](char c) { return c == ','; });
    if(commas == 2){
        size_t pos = color_str.find("[");
        color_str.replace(pos,1,"rgb(");
        pos = color_str.find("]");
        color_str.replace(pos,1,")");
        return color_str;
    }
    if(commas == 3){
        size_t pos = color_str.find("[");
        color_str.replace(pos,1,"rgba(");
        pos = color_str.find("]");
        color_str.replace(pos,1,")");
        return color_str;
    }

    return color_str;
}




}//end namespace
