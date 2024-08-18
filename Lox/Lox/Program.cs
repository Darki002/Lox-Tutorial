﻿namespace Lox;

internal static class Program
{
    private static bool hadError;
    
    public static void Main(string[] args)
    {
        if (args.Length > 1)
        {
            Console.WriteLine("Usage: jlox [script]");
            Environment.Exit(64);
        }       
        else if (args.Length == 1)
        {
            RunFile(args.First());
        }
        else
        {
            RunPrompt();
        }
    }

    private static void RunPrompt()
    {
        while (true)
        {
            var line = Console.ReadLine();
            if(line is "exit" or null) break;
            Run(line);
            hadError = false;
        }
    }
    
    private static void RunFile(string fileName)
    {
        var content = File.ReadAllText(fileName);
        Run(content);
        
        if(hadError) Environment.Exit(64);
    }

    private static void Run(string source)
    {
        var scanner = new Scanner(source);
        List<Token> tokens = scanner.ScanTokens();

        foreach (var token in tokens)
        {
            Console.WriteLine(token);
        }
    }

    private static void Error(int line, string message)
    {
        Report(line, string.Empty, message);
    }

    private static void Report(int line, string where, string message)
    {
        Console.WriteLine($"[Line {line}] Error {where}: {message}");
        hadError = true;
    }
}