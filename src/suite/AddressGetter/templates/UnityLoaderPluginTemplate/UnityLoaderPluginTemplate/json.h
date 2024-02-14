#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

struct OffsetRequest
{
    std::string searchName;
    std::string value;
    std::string signature;
};

struct AddressGetterRequest
{
    std::string caller;
    std::vector<OffsetRequest> offsetRequests;
};

// Function to read JSON data from a file and return a AddressGetterRequest struct
AddressGetterRequest readJsonFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    nlohmann::json jsonData;
    file >> jsonData;

    AddressGetterRequest example;
    example.caller = jsonData["caller"];

    for (const auto& offsetReq : jsonData["offsetRequests"])
    {
        OffsetRequest req;
        req.searchName = offsetReq["searchName"];
        req.value = offsetReq["value"];
        req.signature = offsetReq["signature"];
        example.offsetRequests.push_back(req);
    }

    file.close();
    return example;
}
