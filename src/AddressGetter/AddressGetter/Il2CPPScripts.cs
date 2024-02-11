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
            // Simple demangling for the provided examples
            str = Regex.Replace(str, @"^<.*>\$\$.*_", "");
            str = Regex.Replace(str, @"\$\$", "_");
            str = Regex.Replace(str, @"__", "_");
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
            string offsetsHeader = location + $"\\{projectName}" + "\\offsets.h";

            //Chage MODNAME in hooks.h
            string hooksHeaderContent = File.ReadAllText(hooksHeader);
            hooksHeaderContent = hooksHeaderContent.Replace("\"[ModName]\"", $"\"[{projectName}]\"");
            File.WriteAllText(hooksHeader, hooksHeaderContent);

            List<string> methodNamesDemangled = new();

            using (StreamWriter sw = File.AppendText(offsetsHeader))
            {
                //Add all addresses of methods to offsets.h
                foreach (ScriptMethod scriptMethod in scriptMethods)
                {
                    string methodAddress = scriptMethod.AddressHex;
                    string methodName = scriptMethod.DemangledName;
                    methodNamesDemangled.Add(methodName);
                    if (!methodName.Contains("<>"))
                        sw.WriteLine("uintptr_t {0} = {1};", methodName, methodAddress);
                }
            }

            string callerRequestsJsonFile = location + "\\" + projectName + "\\" + projectName + ".offsetRequests.json";

            List<Requests.OffSetRequest> offsetRequests = new();
            foreach (var name in methodNamesDemangled)
            {
                offsetRequests.Add(new Requests.OffSetRequest
                {
                    Value = "0xFFFFFF",
                    SearchName = name
                });
            }

            Requests.CallerRequests callerRequests = new Requests.CallerRequests
            {
                Caller = $"{projectName}",
                OffsetRequests = offsetRequests.ToArray()
            };

            File.WriteAllText(callerRequestsJsonFile, JsonSerializer.Serialize(callerRequests));
        }
    }
}