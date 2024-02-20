using System.Text.Json;
using System.Text.RegularExpressions;

namespace AddressGetter;

public class Il2CPPScripts
{
    public class DumpedScriptMethods
    {
        public ScriptMethod[] ScriptMethod { get; set; }
    }

    public class ScriptMethod
    {
        public int Address { get; set; }
        public string AddressHex { get; set; }
        public string Name { get; set; }
        public string DemangledName { get; set; }
        public string Signature { get; set; }
        public string TypeSignature { get; set; }

        public ScriptMethod(int address, string name, string signature, string typeSignature)
        {
            Address = address;
            AddressHex = $"0x{address:X}";
            Name = name;
            Signature = signature;
            TypeSignature = typeSignature;
            DemangledName = Demangle(name);
        }

        public static string Demangle(string str)
        {
            str = Regex.Replace(str, @"^<.*>\$\$.*_", "");
            str = Regex.Replace(str, @"\$\$", "_");
            str = Regex.Replace(str, @"__", "_");

            //Replace all remaining dots with underscores
            str = Regex.Replace(str, @"\.", "_");

            return str;
        }
    }

    public static class ScriptMethodExtensions
    {
        private static void CopyFilesRecursively(string sourcePath, string targetPath)
        {
            if (!Directory.Exists(targetPath)) Directory.CreateDirectory(targetPath);

            //Now Create all of the directories
            foreach (string dirPath in Directory.GetDirectories(sourcePath, "*", SearchOption.AllDirectories))
            {
                Directory.CreateDirectory(dirPath.Replace(sourcePath, targetPath));
            }

            //Copy all the files & Replaces any files with the same name
            foreach (string newPath in Directory.GetFiles(sourcePath, "*.*", SearchOption.AllDirectories))
            {
                File.Copy(newPath, newPath.Replace(sourcePath, targetPath), true);
            }
        }

        public static void CreateCppHookFromMethods(ScriptMethod[] scriptMethods, string location)
        {
            string templatePath = "templates\\UnityLoaderPluginTemplate";
            string projectName = location.Split('\\')[location.Split('\\').Length - 1];
            CopyFilesRecursively(templatePath, location);

            //Rename all folders that contain UnitLoaderPluginTemplate to the project name
            foreach (string dirPath in Directory.GetDirectories(location, "UnityLoaderPluginTemplate*",
                         SearchOption.AllDirectories))
            {
                Directory.Move(dirPath, dirPath.Replace("UnityLoaderPluginTemplate", projectName));
            }

            //Rename project in sln file
            string slnPath = location + "\\UnityLoaderPluginTemplate.sln";
            string slnContent = File.ReadAllText(slnPath);
            slnContent = slnContent.Replace("UnityLoaderPluginTemplate",
                projectName);
            string newSlnPath = location + "\\" + projectName + ".sln";
            File.WriteAllText(newSlnPath, slnContent);
            File.Delete(slnPath);

            //Rename project in vcxproj file
            string vcxprojPath = location + $"\\{projectName}" + "\\UnityLoaderPluginTemplate.vcxproj";
            string vcxprojContent = File.ReadAllText(vcxprojPath);
            vcxprojContent = vcxprojContent.Replace("UnityLoaderPluginTemplate",
                projectName);
            string newVcxprojPath = location + "\\" + projectName + $"\\{projectName}.vcxproj";
            File.WriteAllText(newVcxprojPath, vcxprojContent);
            File.Delete(vcxprojPath);

            //rename vcxproj.filters file
            string vcxprojFiltersPath = location + $"\\{projectName}" + "\\UnityLoaderPluginTemplate.vcxproj.filters";
            string vcxprojFiltersContent = File.ReadAllText(vcxprojFiltersPath);
            vcxprojFiltersContent = vcxprojFiltersContent.Replace("UnityLoaderPluginTemplate",
                projectName);
            string newVcxprojFiltersPath = location + "\\" + projectName + $"\\{projectName}.vcxproj.filters";
            File.WriteAllText(newVcxprojFiltersPath, vcxprojFiltersContent);
            File.Delete(vcxprojFiltersPath);

            //rename UnityLoaderPluginTemplate.rc file
            string rcPath = location + $"\\{projectName}" + "\\UnityLoaderPluginTemplate.rc";
            string rcContent = File.ReadAllText(rcPath);
            rcContent = rcContent.Replace("UnityLoaderPluginTemplate",
                projectName);
            string newRcPath = location + "\\" + projectName + $"\\{projectName}.rc";
            File.WriteAllText(newRcPath, rcContent);
            File.Delete(rcPath);

            string hooksHeader = location + $"\\{projectName}" + "\\hooks.h";
            string callerRequestsJsonFile = location + "\\" + projectName + "\\" + projectName + ".offsetRequests.json";
            string hooksHeaderContent = File.ReadAllText(hooksHeader);

            string includesHeader = location + $"\\{projectName}" + "\\includes.h";
            string includesHeaderContent = File.ReadAllText(includesHeader);

            includesHeaderContent = includesHeaderContent.Replace("\"[ModName]\"", $"\"[{projectName}]\"").Replace(
                "\"\\\\Plugins\\\\ModName.offsetRequests.json\"",
                $"\"\\\\Plugins\\\\{projectName}.offsetRequests.json\"");

            File.WriteAllText(includesHeader, includesHeaderContent);

            List<Requests.OffSetRequest> offsetRequests = new();
            List<string> hooks = new();
            List<string> hookCreations = new();

            bool createAutoHooks;
            Console.WriteLine(
                "Do you want to auto generate hooks for the functions (y/n)?\nThis can be buggy and may need manual editing!");
            string autoGenYesNo = Console.ReadLine() ?? "n";
            createAutoHooks = autoGenYesNo.ToLower() == "y";
            for (var index = 0; index < scriptMethods.Length; index++)
            {
                var method = scriptMethods[index];
                // Continue with next iteration if method contains some mangled stuff, idk how safe this is, but it works...
                // for now
                if (method.DemangledName.Contains('<')) continue;

                // Define regular expressions for extracting information
                string returnTypePattern = @"(\w+)\s+"; // Matches the return type
                string signatureTypesPattern = @"\((.*?)\)"; // Matches everything inside the parentheses
                string signatureTypesWithNamesPattern = @"\b\w+\b(?:(?:,\s*)|$)"; // Matches only the names

                // Extract return type
                Match returnTypeMatch = Regex.Match(method.Signature, returnTypePattern);
                string returnType = returnTypeMatch.Success ? returnTypeMatch.Groups[1].Value : "";

                // Extract signature types
                Match signatureTypesMatch = Regex.Match(method.Signature, signatureTypesPattern);
                string signatureTypes = signatureTypesMatch.Success ? signatureTypesMatch.Groups[1].Value : "";

                // Extract signature types with names
                MatchCollection signatureTypesWithNamesMatches =
                    Regex.Matches(method.Signature, signatureTypesWithNamesPattern);

                string namesOnly = string.Join(" ", signatureTypesWithNamesMatches
                    .Cast<Match>()
                    .Select(match => match.Value.Trim()));

                string signatureTypesWithoutNames = string.Empty;
                string[] splittedSigTypes = signatureTypes.Split(',');
                foreach (var type in splittedSigTypes)
                {
                    var span = type.AsSpan();
                    int start = 0;
                    int end = span.LastIndexOf(' ');

                    string final = span.Slice(start, end).ToString() + ',';
                    signatureTypesWithoutNames += final;
                }

                string hookCalledNotification = returnType == "void"
                    ? $"\"{method.DemangledName} called\""
                    : $"\"{method.DemangledName} called with result: \" << returnResult";
                string returnVariable = returnType == "void"
                    ? ""
                    : $"{returnType} returnResult = {method.DemangledName}_o({namesOnly} method);";
                string returnStatement = returnType == "void"
                    ? $"{method.DemangledName}_o({namesOnly} method)"
                    : "returnResult";
                hooks.Add(new string($@"

using {method.DemangledName}_t = {returnType}(__fastcall*)({signatureTypesWithoutNames.Remove(signatureTypesWithoutNames.LastIndexOf(','))});
{method.DemangledName}_t {method.DemangledName}_o; 

{returnType} __stdcall {method.DemangledName}_hook({signatureTypes})
{{
    {returnVariable}
    Unity::CComponent* caller = (Unity::CComponent*)__this; //Gets the instance of the current unity gameobject component
    QUICKDEBUG({hookCalledNotification});
    return {returnStatement};
}}"));

                hookCreations.Add(new string($@"
    uintptr_t {method.DemangledName}Offset = std::stoull(functionOffsets[{index}].value, nullptr, 16);
    MH_CreateHook(
        reinterpret_cast<LPVOID*>(gameAsm + {method.DemangledName}Offset),
        &{method.DemangledName}_hook,
        (LPVOID*)&{method.DemangledName}_o);
"));

                offsetRequests.Add(new Requests.OffSetRequest
                {
                    Value = method.AddressHex,
                    SearchName = method.DemangledName,
                    Signature = method.Signature
                });
            }

            Requests.CallerRequests callerRequests = new Requests.CallerRequests
            {
                Caller = $"{projectName}",
                OffsetRequests = offsetRequests.ToArray()
            };

            File.WriteAllText(callerRequestsJsonFile, JsonSerializer.Serialize(callerRequests));

            // Insert hooks after the first #define
            if (createAutoHooks)
            {
                int defineIndex = hooksHeaderContent.IndexOf("*/", StringComparison.Ordinal) + 3;
                hooksHeaderContent = hooksHeaderContent.Insert(defineIndex, string.Join("\n", hooks));

                int instantiationIndex = hooksHeaderContent.LastIndexOf("*/", StringComparison.Ordinal) + 3;
                hooksHeaderContent = hooksHeaderContent.Insert(instantiationIndex, string.Join("\n", hookCreations));
            }

            // Write the modified content back to the file
            File.WriteAllText(hooksHeader, hooksHeaderContent);
        }
    }
}