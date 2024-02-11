﻿using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.RegularExpressions;

namespace AddressGetter;

class Program
{
    private static void DebugLineWithName(string msg)
    {
        Console.WriteLine($"[AdressGetter.exe]: {msg}");
    }

    static void Main(string[] args)
    {
        string debugArgs = string.Empty;

        debugArgs += "Got the arguments: ";
        foreach (var arg in args)
        {
            debugArgs += arg + " ";
        }

        DebugLineWithName(debugArgs);

        try
        {
            if (args.Length > 0)
            {
                if (args[0].Contains("internal"))
                {
                    if (args.Length >= 4)
                    {
                        InternalMain(args);
                    }
                    else
                    {
                        Console.WriteLine("Not enough arguments for internal mode.");
                    }
                }
                else
                {
                    StandaloneMain(args);
                }
            }
            else
            {
                StandaloneMain(args);
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(Environment.NewLine);
            Console.WriteLine("================= Exception message =================");
            Console.WriteLine("An error during address getting occured: " + e.Message);
            Console.WriteLine("================= Exception message end =================");
            Console.WriteLine(Environment.NewLine);
            Console.WriteLine("================= Exception trace =================");
            Console.WriteLine(e.StackTrace);
            Console.WriteLine("================= Exception trace end =================");
            Console.WriteLine(Environment.NewLine);
        }
    }


    static void InternalMain(string[] args)
    {
        string requestFile = args[1];
        string scriptsJson = args[2] != "useDefault" ? args[2] : Path.GetFullPath(@".\DumpedIL2CPP\script.json");
        bool stealthPrint = bool.Parse(args[3]);

        Requests.CallerRequests callerRequests =
            JsonSerializer.Deserialize<Requests.CallerRequests>(
                File.ReadAllText(requestFile))!;

        DebugLineWithName(
            $"{callerRequests.Caller} is requesting {callerRequests.OffsetRequests.Length} offsets, starting return process!");

        Il2CPPScripts.DumpedScriptMethods dumpedScriptMethods = JsonSerializer.Deserialize<Il2CPPScripts
            .DumpedScriptMethods>(File.ReadAllText(scriptsJson))!;

        foreach (var method in dumpedScriptMethods.ScriptMethod)
        {
            string name = method.DemangledName;
            foreach (var request in callerRequests.OffsetRequests)
            {
                if (name == request.SearchName)
                {
                    if (stealthPrint)
                    {
                        DebugLineWithName(method.AddressHex);
                        request.Value = method.AddressHex;
                        continue;
                    }

                    DebugLineWithName($"Found one request of {name} with address {method.AddressHex} in {scriptsJson}");
                }
            }
        }

        string newJson = JsonSerializer.Serialize(callerRequests);
        File.WriteAllText(requestFile, newJson);
    }

    static void StandaloneMain(string[] args)
    {
        string inputFile;
        string searchFor;
        string outputFile;
        Il2CPPScripts.DumpedScriptMethods dumpedScriptMethods;

        if (args.Length < 1)
        {
            Console.WriteLine("Please enter the path to the script.json file:");
            inputFile = Console.ReadLine();

            Console.WriteLine(
                "Please enter the method name you want to search for (separate by a comma if you want to search for multiple methods):");
            searchFor = Console.ReadLine();

            Console.WriteLine("Please enter the output file path (leave empty to skip):");
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

        dumpedScriptMethods =
            JsonSerializer.Deserialize<Il2CPPScripts.DumpedScriptMethods>(
                File.ReadAllText(inputFile));

        Dictionary<string, Il2CPPScripts.ScriptMethod> methods = new();
        foreach (var method in dumpedScriptMethods.ScriptMethod)
        {
            foreach (var mn in searchFor.Split(','))
            {
                if (method.Name.ToLower().Contains(mn.ToLower()))
                {
                    //Skip if already added
                    if (methods.ContainsKey(method.Name + "=" + "0x" + method.AddressHex)) continue;

                    Console.WriteLine("============================== Method found ==============================");
                    methods.Add(method.Name + "=" + "0x" + method.AddressHex, method);
                    Console.WriteLine("Demangled name: " + Il2CPPScripts.ScriptMethod.Demangle(method.Name));
                    Console.WriteLine("Method: " + method.Name + "\nMethod Address: " + method.AddressHex +
                                      Environment.NewLine);
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

            Il2CPPScripts.ScriptMethodExtensions.CreateCppHookFromMethod(methods.Values.ToArray(),
                projectPath + "\\" + projectName);
            Console.WriteLine("Created project!");
        }

        Console.WriteLine("Press any key to exit...");
        Console.ReadKey();
    }
}