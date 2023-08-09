#ifndef MY_HTTP_WEBPAGE_HANDLERS_HPP
#define MY_HTTP_WEBPAGE_HANDLERS_HPP

#include <string>
#include <map>

#include "esp_http_server.h"

// Embedded file: index.html
extern const uint8_t index_html_gz_start[] asm("_binary_index_html_gz_start");
extern const uint8_t index_html_gz_end[] asm("_binary_index_html_gz_end");

// Embedded file: style.css
extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");

// Embedded file: favicon.ico
extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

// Embedded file: manifest.json
extern const uint8_t manifest_json_start[] asm("_binary_manifest_json_start");
extern const uint8_t manifest_json_end[] asm("_binary_manifest_json_end");

// Embedded file: icons.woff2
// extern const uint8_t icons_woff2_start[] asm("_binary_icons_woff2_start");
// extern const uint8_t icons_woff2_end[] asm("_binary_icons_woff2_end");

// Embedded file: asset-manifest.json
extern const uint8_t asset_manifest_json_gz_start[] asm("_binary_asset_manifest_json_gz_start");
extern const uint8_t asset_manifest_json_gz_end[] asm("_binary_asset_manifest_json_gz_end");

// Embedded file: chunk.27.js
extern const uint8_t chunk_27_js_gz_start[] asm("_binary_chunk_27_js_gz_start");
extern const uint8_t chunk_27_js_gz_end[] asm("_binary_chunk_27_js_gz_end");

// Embedded file: main.js
extern const uint8_t main_js_gz_start[] asm("_binary_main_js_gz_start");
extern const uint8_t main_js_gz_end[] asm("_binary_main_js_gz_end");

// Embedded file: add.svg
extern const uint8_t add_svg_gz_start[] asm("_binary_add_svg_gz_start");
extern const uint8_t add_svg_gz_end[] asm("_binary_add_svg_gz_end");

// Embedded file: crazybees.svg
extern const uint8_t crazybees_svg_gz_start[] asm("_binary_crazybees_svg_gz_start");
extern const uint8_t crazybees_svg_gz_end[] asm("_binary_crazybees_svg_gz_end");

// Embedded file: fire.svg
extern const uint8_t fire_svg_gz_start[] asm("_binary_fire_svg_gz_start");
extern const uint8_t fire_svg_gz_end[] asm("_binary_fire_svg_gz_end");

// Embedded file: matrix.svg
extern const uint8_t matrix_svg_gz_start[] asm("_binary_matrix_svg_gz_start");
extern const uint8_t matrix_svg_gz_end[] asm("_binary_matrix_svg_gz_end");

// Embedded file: plasmawaves.svg
extern const uint8_t plasmawaves_svg_gz_start[] asm("_binary_plasmawaves_svg_gz_start");
extern const uint8_t plasmawaves_svg_gz_end[] asm("_binary_plasmawaves_svg_gz_end");

// Embedded file: 
extern const uint8_t rainbow_svg_gz_start[] asm("_binary_rainbow_svg_gz_start");
extern const uint8_t rainbow_svg_gz_end[] asm("_binary_rainbow_svg_gz_end");

// Embedded file: sparkles.svg
extern const uint8_t sparkles_svg_gz_start[] asm("_binary_sparkles_svg_gz_start");
extern const uint8_t sparkles_svg_gz_end[] asm("_binary_sparkles_svg_gz_end");

// Embedded file: waterfall.svg
extern const uint8_t waterfall_svg_gz_start[] asm("_binary_waterfall_svg_gz_start");
extern const uint8_t waterfall_svg_gz_end[] asm("_binary_waterfall_svg_gz_end");

// Embedded file: snowfall.svg
extern const uint8_t snowfall_svg_gz_start[] asm("_binary_snowfall_svg_gz_start");
extern const uint8_t snowfall_svg_gz_end[] asm("_binary_snowfall_svg_gz_end");

// Embedded file: palette.svg
extern const uint8_t palette_svg_gz_start[] asm("_binary_palette_svg_gz_start");
extern const uint8_t palette_svg_gz_end[] asm("_binary_palette_svg_gz_end");

// Embedded file: close.svg
extern const uint8_t close_svg_gz_start[] asm("_binary_close_svg_gz_start");
extern const uint8_t close_svg_gz_end[] asm("_binary_close_svg_gz_end");

// Embedded file: menu.svg
extern const uint8_t menu_svg_start[] asm("_binary_menu_svg_start");
extern const uint8_t menu_svg_end[] asm("_binary_menu_svg_end");

// Embedded file: chevron.svg
extern const uint8_t chevron_svg_start[] asm("_binary_chevron_svg_start");
extern const uint8_t chevron_svg_end[] asm("_binary_chevron_svg_end");

struct BinHandlerData{
    const uint8_t* start;
    const uint8_t* end;
    const char* type;
    std::string encoding{"gzip"};
};

static const std::map<std::string, BinHandlerData> handlers{
    {"/", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/effects", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/text", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/clock", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/entertainment", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/timer", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/weather", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/settings", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/sync", {index_html_gz_start, index_html_gz_end, "text/html"}},
    {"/style.css", {style_css_start, style_css_end, "text/css", ""}},
    {"/favicon.ico", {favicon_ico_start, favicon_ico_end, "image/x-icon", ""}},
    // {"/icons.woff2", {icons_woff2_start, icons_woff2_end, "font/woff2"}},
    {"/manifest.json", {manifest_json_start, manifest_json_end, "application/json", ""}},
    {"/asset-manifest.json", {asset_manifest_json_gz_start, asset_manifest_json_gz_end, "application/json"}},
    {"/static/js/chunk.27.js", {chunk_27_js_gz_start, chunk_27_js_gz_end, "application/javascript"}},
    {"/static/js/main.js", {main_js_gz_start, main_js_gz_end, "application/javascript"}},
    {"/static/media/add.svg", {add_svg_gz_start, add_svg_gz_end, "image/svg+xml"}},
    {"/static/media/crazybees.svg", {crazybees_svg_gz_start, crazybees_svg_gz_end, "image/svg+xml"}},
    {"/static/media/fire.svg", {fire_svg_gz_start, fire_svg_gz_end, "image/svg+xml"}},
    {"/static/media/matrix.svg", {matrix_svg_gz_start, matrix_svg_gz_end, "image/svg+xml"}},
    {"/static/media/plasmawaves.svg", {plasmawaves_svg_gz_start, plasmawaves_svg_gz_end, "image/svg+xml"}},
    {"/static/media/rainbow.svg", {rainbow_svg_gz_start, rainbow_svg_gz_end, "image/svg+xml"}},
    {"/static/media/sparkles.svg", {sparkles_svg_gz_start, sparkles_svg_gz_end, "image/svg+xml"}},
    {"/static/media/waterfall.svg", {waterfall_svg_gz_start, waterfall_svg_gz_end, "image/svg+xml"}},
    {"/static/media/snowfall.svg", {snowfall_svg_gz_start, snowfall_svg_gz_end, "image/svg+xml"}},

    {"/static/media/palette.svg", {palette_svg_gz_start, palette_svg_gz_end, "image/svg+xml"}},
    {"/static/media/close.svg", {close_svg_gz_start, close_svg_gz_end, "image/svg+xml"}},
    {"/static/media/menu.svg", {menu_svg_start, menu_svg_end, "image/svg+xml", ""}},
    {"/static/media/chevron.svg", {chevron_svg_start, chevron_svg_end, "image/svg+xml", ""}},

};

extern "C" esp_err_t http_server_webpage_handler(httpd_req_t* req)
{    
    if(const auto it = handlers.find(req->uri); it != handlers.end())
    {        
        httpd_resp_set_type(req, it->second.type);
        if(it->second.encoding != "")
            httpd_resp_set_hdr(req, "Content-Encoding", it->second.encoding.c_str());
        httpd_resp_send(req, (const char*) it->second.start, it->second.end - it->second.start);
        return ESP_OK;
    }

    httpd_resp_send_404(req);
    return ESP_ERR_NOT_FOUND;
}

void http_server_register_webpage(httpd_handle_t http_server_handle)
{
    for(const auto& [uri, data] : handlers)
    {
        httpd_uri_t reqHandler{
            uri.c_str(),                        // URI
            HTTP_GET,                           // Method: GET
            &http_server_webpage_handler,       // Handler funciton
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &reqHandler);
    }
}

#endif