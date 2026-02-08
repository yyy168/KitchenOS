#include "include/RecipeManager.hpp"
#include <cassert>
#include <iostream>

void testSearchCaseInsensitivity() {
    RecipeService service;
    service.addRecipe({0, 1, "Spaghetti", "Cook it.", "2p"});
    // Verify both upper and partial matches work
    assert(service.search(1, "SPAGHETTI").size() == 1);
    assert(service.search(1, "ghetti").size() == 1);
}

void testMultiTenancy() {
    RecipeService service;
    service.addRecipe({0, 1, "RestA_Dish", "Prep", "1p"});
    service.addRecipe({0, 2, "RestB_Dish", "Prep", "1p"});
    // Verify Restaurant 1 cannot see Restaurant 2's data
    assert(service.search(1, "").size() == 1);
    assert(service.search(1, "RestB").size() == 0);
}

void testRecipeDeletion() {
    RecipeService service;
    service.addRecipe({0, 1, "Temp Dish", "Delete me", "1p"});
    
    // Get the assigned ID from the search results
    auto recipes = service.search(1, ""); 
    int assigned_id = recipes[0].id; 
    
    bool deleted = service.deleteRecipe(1, assigned_id);
    assert(deleted == true);
    assert(service.search(1, "").size() == 0);
    std::cout << "✅ Recipe Deletion Logic Passed" << std::endl;
}

void testDataIntegrity() {
    RecipeService service;
    service.addRecipe({0, 1, "Yield Test", "Instructions", "4 Servings"});
    auto results = service.search(1, "Yield Test");
    
    // Verify all fields are preserved correctly
    assert(results[0].yield == "4 Servings");
    assert(results[0].instructions == "Instructions");
}

int main() {
    try {
        testSearchCaseInsensitivity();
        testMultiTenancy();
        testRecipeDeletion();
        testDataIntegrity();
    } catch (const std::exception& e) {
        std::cerr << "❌ Tests failed with error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Tests failed with an unknown exception." << std::endl;
        return 1;
    }
    return 0;
}