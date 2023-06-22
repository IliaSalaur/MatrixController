#ifndef MY_HTTP_WEBPAGE_HANDLERS_HPP
#define MY_HTTP_WEBPAGE_HANDLERS_HPP

#include <string>
#include <map>

#include "esp_http_server.h"

// Embedded file: index.html
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

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
extern const uint8_t icons_woff2_start[] asm("_binary_icons_woff2_start");
extern const uint8_t icons_woff2_end[] asm("_binary_icons_woff2_end");

// Embedded file: asset-manifest.json
extern const uint8_t asset_manifest_json_start[] asm("_binary_asset_manifest_json_start");
extern const uint8_t asset_manifest_json_end[] asm("_binary_asset_manifest_json_end");

// Embedded file: chunk.27.js
extern const uint8_t chunk_27_js_start[] asm("_binary_chunk_27_js_start");
extern const uint8_t chunk_27_js_end[] asm("_binary_chunk_27_js_end");

// Embedded file: main.js
extern const uint8_t main_js_start[] asm("_binary_main_js_start");
extern const uint8_t main_js_end[] asm("_binary_main_js_end");

// Embedded file: add.svg
extern const uint8_t add_svg_start[] asm("_binary_add_svg_start");
extern const uint8_t add_svg_end[] asm("_binary_add_svg_end");

// Embedded file: crazybees.png
extern const uint8_t crazybees_png_start[] asm("_binary_crazybees_png_start");
extern const uint8_t crazybees_png_end[] asm("_binary_crazybees_png_end");

// Embedded file: fire.png
extern const uint8_t fire_png_start[] asm("_binary_fire_png_start");
extern const uint8_t fire_png_end[] asm("_binary_fire_png_end");

// Embedded file: matrix.png
extern const uint8_t matrix_png_start[] asm("_binary_matrix_png_start");
extern const uint8_t matrix_png_end[] asm("_binary_matrix_png_end");

// Embedded file: plasmawaves.png
extern const uint8_t plasmawaves_png_start[] asm("_binary_plasmawaves_png_start");
extern const uint8_t plasmawaves_png_end[] asm("_binary_plasmawaves_png_end");

// Embedded file: 
extern const uint8_t rainbow_png_start[] asm("_binary_rainbow_png_start");
extern const uint8_t rainbow_png_end[] asm("_binary_rainbow_png_end");

// Embedded file: sparkles.png
extern const uint8_t sparkles_png_start[] asm("_binary_sparkles_png_start");
extern const uint8_t sparkles_png_end[] asm("_binary_sparkles_png_end");

// Embedded file: waterfall.png
extern const uint8_t waterfall_png_start[] asm("_binary_waterfall_png_start");
extern const uint8_t waterfall_png_end[] asm("_binary_waterfall_png_end");

struct BinHandlerData{
    const uint8_t* start;
    const uint8_t* end;
    const char* type;
};

static const std::map<std::string, BinHandlerData> handlers{
    {"/", {index_html_start, index_html_end, "text/html"}},
    {"/effects", {index_html_start, index_html_end, "text/html"}},
    {"/text", {index_html_start, index_html_end, "text/html"}},
    {"/clock", {index_html_start, index_html_end, "text/html"}},
    {"/entertainment", {index_html_start, index_html_end, "text/html"}},
    {"/timer", {index_html_start, index_html_end, "text/html"}},
    {"/weather", {index_html_start, index_html_end, "text/html"}},
    {"/settings", {index_html_start, index_html_end, "text/html"}},
    {"/style.css", {style_css_start, style_css_end, "text/css"}},
    {"/favicon.ico", {favicon_ico_start, favicon_ico_end, "image/x-icon"}},
    {"/icons.woff2", {icons_woff2_start, icons_woff2_end, "font/woff2"}},
    {"/manifest.json", {manifest_json_start, manifest_json_end, "application/json"}},
    {"/asset-manifest.json", {asset_manifest_json_start, asset_manifest_json_end, "application/json"}},
    {"/static/js/chunk.27.js", {chunk_27_js_start, chunk_27_js_end, "application/javascript"}},
    {"/static/js/main.js", {main_js_start, main_js_end, "application/javascript"}},
    {"/static/media/add.svg", {add_svg_start, add_svg_end, "image/svg+xml"}},
    {"/static/media/crazybees.png", {crazybees_png_start, crazybees_png_end, "image/png"}},
    {"/static/media/fire.png", {fire_png_start, fire_png_end, "image/png"}},
    {"/static/media/matrix.png", {matrix_png_start, matrix_png_end, "image/png"}},
    {"/static/media/plasmawaves.png", {plasmawaves_png_start, plasmawaves_png_end, "image/png"}},
    {"/static/media/rainbow.png", {rainbow_png_start, rainbow_png_end, "image/png"}},
    {"/static/media/sparkles.png", {sparkles_png_start, sparkles_png_end, "image/png"}},
    {"/static/media/waterfall.png", {waterfall_png_start, waterfall_png_end, "image/png"}},

};

extern "C" esp_err_t http_server_webpage_handler(httpd_req_t* req)
{    
    if(const auto it = handlers.find(req->uri); it != handlers.end())
    {        
        httpd_resp_set_type(req, it->second.type);
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