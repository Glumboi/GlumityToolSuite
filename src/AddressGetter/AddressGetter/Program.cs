using System.Globalization;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Xml.XPath;


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

    public static void CreateCppHookFromMethod(ScriptMethod[] scriptMethods, string location)
    {
        string projectName = location.Split('\\')[location.Split('\\').Length - 1];
        CopyFilesRecursively("templates\\UnityLoaderPluginTemplate", location);

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

        using (StreamWriter sw = File.AppendText(offsetsHeader))
        {
            //Add all addresses of methods to offsets.h
            foreach (ScriptMethod scriptMethod in scriptMethods)
            {
                string methodAddress = scriptMethod.AddressHex;
                string methodName = scriptMethod.DemangledName;
                if (!methodName.Contains("<>"))
                    sw.WriteLine("uintptr_t {0} = {1};", methodName, methodAddress);
            }
        }
    }
}


class Program
{
    static void Main(string[] args)
    {
        string inputFile;
        string searchFor;
        string outputFile;
        DumpedFunctionClass dumpedFunctionClass;

        if (args.Length < 1)
        {
            Console.WriteLine("Please enter the path to the script.json file:");
            inputFile = Console.ReadLine();

            Console.WriteLine(
                "Please enter the method name you want to search for (seperatew by a comma if you want to search for multiple methods):");
            searchFor = Console.ReadLine();

            Console.WriteLine("Please enter the output file path:");
            outputFile = Console.ReadLine();

            //Remove all double quotes 
            inputFile = inputFile.Replace("\"", "");
            outputFile = outputFile.Replace("\"", "");
            searchFor = searchFor.Replace("\"", "");
        }
        else
        {
            inputFile = args[0];
            searchFor = args[1];
            outputFile = args[2];
        }

        dumpedFunctionClass =
            JsonSerializer.Deserialize<DumpedFunctionClass>(
                File.ReadAllText(inputFile));

        Dictionary<string, ScriptMethod> methods = new();
        foreach (var method in dumpedFunctionClass.ScriptMethod)
        {
            foreach (var mn in searchFor.Split(','))
            {
                if (method.Name.ToLower().Contains(mn.ToLower()))
                {
                    //Skip if already added
                    if (methods.ContainsKey(method.Name + "=" + "0x" + method.AddressHex)) continue;
                    methods.Add(method.Name + "=" + "0x" + method.AddressHex, method);
                    Console.WriteLine(ScriptMethod.Demangle(method.Name));
                    Console.WriteLine("Method: " + method.Name + "\nMethod Address: " + method.AddressHex);
                }
            }
        }

        if (methods.Count < 1) Console.WriteLine("Method not found!");

        if (!string.IsNullOrWhiteSpace(outputFile))
        {
            File.WriteAllLines(outputFile, methods.Keys);
        }

        Console.WriteLine("Create project for searched methods? (y/n)");
        string createProject = Console.ReadLine();
        if (createProject.ToLower() == "y")
        {
            Console.WriteLine("Enter project name:");
            string projectName = Console.ReadLine();
            Console.WriteLine("Enter project path:");
            string projectPath = Console.ReadLine();

            ScriptMethodExtensions.CreateCppHookFromMethod(methods.Values.ToArray(), projectPath + "\\" + projectName);
            Console.WriteLine("Created project!");
        }

        Console.ReadKey();
    }
}

public class DumpedFunctionClass
{
    public ScriptMethod[] ScriptMethod { get; set; }
}