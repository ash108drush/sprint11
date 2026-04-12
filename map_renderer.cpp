#include "map_renderer.h"
#include <iostream>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
namespace transport_catalogue {

void MapRenderer::DrawLine(std::string_view name,const domain::Stop* stop1, const domain::Stop * stop2){
    out_ << stop1->name << "to: " << stop2->name << std::endl;

    out_ << "drawLine" << stop1->coordinates.lat << stop1->coordinates.lng
              << "to:  " << stop2->coordinates.lat << stop2->coordinates.lng << std::endl;
}

void MapRenderer::RenderMap(const RenderSettings &render_settings)
{

}


}//end namespace
