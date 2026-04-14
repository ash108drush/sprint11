#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
using namespace transport_catalogue;

class RequestHandler {
public:
    RequestHandler(const main::TransportCatalogue& db):db_(db){};

    std::optional<main::BusStat> GetBusStat(const std::string_view name) const;
    std::optional<std::set<std::string_view>> GetStopInfo(std::string_view name) const;
    void DrawBusRoute(std::unique_ptr<MapRenderer>&& map_renderer_ptr) const;
private:
    const main::TransportCatalogue& db_;
};

