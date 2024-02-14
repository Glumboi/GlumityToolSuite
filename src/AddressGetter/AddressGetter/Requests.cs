using System.Text.Json.Serialization;

namespace AddressGetter;

public class Requests
{
    public class OffSetRequest
    {
        [JsonPropertyName("searchName")] public string SearchName { get; set; }
        [JsonPropertyName("value")] public string Value { get; set; }
        [JsonPropertyName("signature")] public string Signature { get; set; }
    }

    public class CallerRequests
    {
        [JsonPropertyName("caller")] public string Caller { get; set; }
        [JsonPropertyName("offsetRequests")] public OffSetRequest[] OffsetRequests { get; set; }
    }
}