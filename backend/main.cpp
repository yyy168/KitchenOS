#include "httplib.h"
#include "RecipeManager.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

int main() {
    httplib::Server svr;
    RecipeService service;

    // 1. Seed Data
    service.addRecipe({
        0, 1, "Classic Carbonara", 
        "Spaghetti, Eggs, Guanciale, Pecorino Romano, Black Pepper",
        "1. Boil pasta. 2. Whisk eggs and cheese. 3. Combine with pasta and guanciale.", 
        "2 Portions"
    });

    service.addRecipe({
        0, 1, "Tomato Basil Soup", 
        "Tomatoes, Fresh Basil, Heavy Cream, Garlic, Onion",
        "1. Roast tomatoes. 2. Blend with basil and cream. 3. Simmer.", 
        "4 Servings"
    });

    service.addRecipe({
        0, 1, "Chocolate Brownies", 
        "Dark Chocolate, Butter, Sugar, Eggs, Flour, Cocoa Powder",
        "1. Mix dry ingredients. 2. Fold in melted butter. 3. Bake at 180C for 25m.", 
        "12 Squares"
    });

    // 2. Serve Frontend
    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        std::ifstream ifs("frontend/index.html");
        if (ifs.is_open()) {
            res.set_content(std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>()), "text/html");
        } else {
            res.status = 404; 
            res.set_content("HTML File Not Found", "text/plain");
        }
    });

    // 3. GET: Search recipes
    svr.Get("/api/recipes", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int res_id = req.has_header("X-Restaurant-ID") ? std::stoi(req.get_header_value("X-Restaurant-ID")) : 1;
            std::string q = req.has_param("q") ? req.get_param_value("q") : "";
            auto results = service.search(res_id, q);
            
            std::string json = "[";
            for (size_t i = 0; i < results.size(); ++i) {
                json += "{\"id\":" + std::to_string(results[i].id) + 
                        ",\"title\":\"" + results[i].title + "\"" +
                        ",\"ingredients\":\"" + results[i].ingredients + "\"" + // New field
                        ",\"yield\":\"" + results[i].yield + "\"" +
                        ",\"instructions\":\"" + results[i].instructions + "\"}";
                if (i < results.size() - 1) json += ",";
            }
            json += "]";
            
            res.set_header("Content-Type", "application/json");
            res.set_content(json, "application/json");
        } catch (...) { 
            res.status = 400; 
        }
    });

    // 4. POST: Add new recipe
    svr.Post("/api/recipes", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int res_id = std::stoi(req.get_header_value("X-Restaurant-ID"));
            
            auto extract = [&](std::string key) {
                size_t pos = req.body.find("\"" + key + "\":\"");
                if (pos == std::string::npos) return std::string("");
                size_t start = pos + key.length() + 4;
                size_t end = req.body.find("\"", start);
                return req.body.substr(start, end - start);
            };
            
            // service.addRecipe now returns the new ID
            int newId = service.addRecipe({
                0, res_id, extract("title"), extract("ingredients"), extract("instructions"), extract("yield")
            });
            
            if (newId != -1) {
                res.status = 201; // Created
                res.set_header("Content-Type", "application/json");
                res.set_content("{\"id\":" + std::to_string(newId) + ",\"status\":\"ok\"}", "application/json");
            } else {
                res.status = 400;
            }
        } catch (...) { 
            res.status = 400; 
        }
    });

    // 5. DELETE: Remove recipe
    svr.Delete(R"(/api/recipes/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int res_id = std::stoi(req.get_header_value("X-Restaurant-ID"));
            int rec_id = std::stoi(req.matches[1]);
            
            if (service.deleteRecipe(res_id, rec_id)) {
                res.status = 200; 
                res.set_header("Content-Type", "application/json");
                res.set_content("{\"status\":\"deleted\"}", "application/json");
            } else {
                res.status = 404;
            }
        } catch (...) { 
            res.status = 400; 
        }
    });

    std::cout << "✅ KitchenOS Backend is live on port 8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}