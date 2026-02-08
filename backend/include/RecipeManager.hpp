#pragma once
#include <string>
#include <vector>
#include <shared_mutex>
#include <mutex> 
#include <algorithm>
#include <cctype>

/**
 * @brief Data model for a kitchen recipe.
 * Uses plain data types for easy serialization to JSON in the API layer.
 */
struct Recipe {
/// @brief Unique system identifier (auto-assigned by RecipeService)
    int id;
    // Owner ID used for multi-tenancy data isolation
    int restaurantId;
    // Descriptive name of the dish (e.g., "Classic Carbonara")
    std::string title;
    // Step-by-step cooking directions
    std::string instructions;
    // Output quantity (e.g., "4 Servings" or "2 Liters")
    std::string yield;
};

/**
 * @brief Thread-safe service for managing recipe data.
 * * Design Decision: Uses a "Reader-Writer Lock" pattern (std::shared_mutex).
 * This allows multiple concurrent 'search' operations (readers) while 
 * ensuring exclusive access for 'add' and 'delete' operations (writers).
 */
class RecipeService {
private:
    /*
     * In-memory data store for recipes.
     *
     * Performance: Uses std::vector for O(1) tail insertion and cache-friendly 
     * contiguous memory allocation.
     *
     * Portability: This in-memory approach ensures the demo is "Zero-Dependency," 
     * avoiding the need to install or configure external database packages 
     * within the Codespace environment. 
     *
     * In a production environment, this would be swapped for a persistent 
     * store like PostgreSQL or SQLite.
     */
    std::vector<Recipe> storage;
    
    // mutable allows locking within 'const-like' search functions
    mutable std::shared_mutex mtx; 
    
    // Internal counter to ensure unique IDs across the system
    int next_id = 0;

    // Helper to normalize strings for case-insensitive comparison.
    std::string toLower(std::string s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), 
            [](unsigned char c){ return std::tolower(c); });
        return result;
    }

public:
    /**
     * @brief Adds a recipe and assigns it a unique system ID.
     * @param r The recipe data (ID field is ignored/overwritten).
     */
    void addRecipe(Recipe r) {
        // Write Lock: Prevents anyone else from reading or writing during insertion
        std::unique_lock lock(mtx);
        r.id = next_id++; 
        storage.push_back(r);
    }

    /**
     * @brief Searches for recipes belonging to a specific restaurant.
     * @param res_id Filter by restaurant ownership (Multi-tenancy).
     * @param query Partial string match for titles (Case-insensitive).
     * @return A filtered vector of recipes.
     */
    std::vector<Recipe> search(int res_id, std::string query) {
        // Read Lock: Multiple threads can search simultaneously
        std::shared_lock lock(mtx);
        std::vector<Recipe> results;
        std::string lowerQuery = toLower(query);

        for (const auto& r : storage) {
            // Multi-tenancy check: ensure data isolation between restaurants
            if (r.restaurantId == res_id) {
                // If query is empty, return all; otherwise, match lowercase title
                if (query.empty() || toLower(r.title).find(lowerQuery) != std::string::npos) {
                    results.push_back(r);
                }
            }
        }
        return results;
    }

    /**
     * @brief Removes a recipe if it exists and belongs to the requesting restaurant.
     * @return true if deleted, false if not found or unauthorized.
     */
    bool deleteRecipe(int res_id, int recipe_id) {
        // Write Lock: Ensures atomic removal to keep indices valid
        std::unique_lock lock(mtx);
        
        // Erase-remove idiom: highly efficient way to delete from std::vector
        auto it = std::remove_if(storage.begin(), storage.end(), [&](const Recipe& r){
            return r.id == recipe_id && r.restaurantId == res_id;
        });
        
        if (it != storage.end()) {
            storage.erase(it, storage.end());
            return true;
        }
        return false;
    }
};