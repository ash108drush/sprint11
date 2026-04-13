#include "map_renderer.h"
#include <iostream>
#include <vector>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
namespace transport_catalogue {



void MapRenderer::RenderMap(StopsMap  bus_stops){
    std::vector<geo::Coordinates> geo_coords;
    geo_coords.reserve(bus_stops.size());
    for(auto const &[bus_name, bus_data] :bus_stops){
        for(auto const &stop: bus_data.first){
            geo_coords.push_back(stop->coordinates);
        }
    }
    const SphereProjector proj{
                               geo_coords.begin(), geo_coords.end(),
                               render_settings_.width,
                               render_settings_.height,
                               render_settings_.padding };

    svg::Document doc;
    std::vector<std::string> color_palette = render_settings_.color_palette;
    VectorStringToRgb(color_palette);
    RenderPolylines(bus_stops,color_palette,proj,doc);
    RenderRouteNames(bus_stops,color_palette,proj,doc);



}

void MapRenderer::RenderRouteNames(const StopsMap & bus_stops,
                                  const std::vector<std::string>& color_palette,
                                  const SphereProjector& proj,
                                  svg::Document& doc){
    int color_index = 0;
    for(auto const &[bus_name, bus_data] :bus_stops){
        svg::Text text = svg::Text();
        svg::Text text_underlayer = svg::Text();

        text.SetFontSize(render_settings_.bus_label_font_size);
        text_underlayer.SetFontSize(render_settings_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text_underlayer.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text_underlayer.SetFontWeight("bold");

        text_underlayer.SetFillColor(render_settings_.underlayer_color);
        text_underlayer.SetStrokeColor(render_settings_.underlayer_color);
        text_underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        text.SetFillColor(color_palette[color_index % color_palette.size()]);


        for(auto const &stop: bus_data.first){
            text_underlayer.SetPosition();
            text_underlayer.SetOffset();
            text.SetPosition();
            text.SetOffset();

        }

        if(bus_data.first.size()  >0 ) {
            doc.Add(text_underlayer);
            doc.Add(text);
            ++color_index;
        }
    }

    doc.Render(out_);

}

void MapRenderer::RenderPolylines(const StopsMap & bus_stops,
                                  const std::vector<std::string>& color_palette,
                                  const SphereProjector& proj,
                                  svg::Document& doc){
    int color_index = 0;
    for(auto const &[bus_name, bus_data] :bus_stops){
        svg::Polyline poly_line = svg::Polyline();
        poly_line.SetFillColor("none");
        poly_line.SetStrokeWidth(render_settings_.line_width);
        poly_line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        poly_line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        poly_line.SetStrokeColor(color_palette[color_index % color_palette.size()]);

        for(auto const &stop: bus_data.first){
            poly_line.AddPoint(proj(stop->coordinates));
        }

        if(bus_data.first.size()  >0 ) {
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
