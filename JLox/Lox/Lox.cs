using Lox.Errors;

namespace Lox;

internal static class Lox
{
    private static readonly Interpreter Interpreter = new Interpreter();
    
    private static bool hadError;
    private static bool hadRuntimeError;

    public static void Main(string[] args)
    {
        if (args.Length > 1)
        {
            Console.WriteLine("Usage: jlox [script]");
            System.Environment.Exit(64);
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
            if (line is "exit" or null) break;
            Run(line);
            hadError = false;
            hadRuntimeError = false;
        }
    }

    private static void RunFile(string fileName)
    {
        var content = File.ReadAllText(fileName);
        Run(content);

        if (hadError) System.Environment.Exit(64);
        if (hadRuntimeError) System.Environment.Exit(70);
    }

    private static void Run(string source)
    {
        // Scanner
        var scanner = new Scanner(source);
        var tokens = scanner.ScanTokens();

        // Parser
        var parser = new Parser(tokens);
        var statements = parser.Parse();
        if (hadError) return;

        // Analyzer
        var analyzer = new Analyzer();
        analyzer.Start(statements);
        if (hadError) return;

        // Resolver
        var resolver = new Resolver(Interpreter);
        resolver.Start(statements);
        if (hadError) return;
        
        // Run
        Interpreter.Interpret(statements!);
    }
    
    public static void Warn(Token token, string message)
    {
        var where = token.Type == TokenType.EOF ? "at end" : $"at '{token.Lexeme}'";
        ReportWarning(token.Line, where, message);
    }

    public static void Error(int line, string message)
    {
        ReportError(line, string.Empty, message);
    }

    public static void Error(Token token, string message)
    {
        var where = token.Type == TokenType.EOF ? "at end" : $"at '{token.Lexeme}'";
        ReportError(token.Line, where, message);
    }

    public static void RuntimeError(RuntimeError runtimeError)
    {
        Console.WriteLine($"[line {runtimeError.Token.Line}] {runtimeError.Message}");
        hadRuntimeError = true;
    }

    private static void ReportError(int line, string where, string message)
    {
        Console.Write($"[Line {line}] ");
        Console.ForegroundColor = ConsoleColor.Red;
        Console.Write("Error");
        Console.ResetColor();
        Console.WriteLine($" {where}: {message}");
        hadError = true;
    }
    
    private static void ReportWarning(int line, string where, string message)
    {
        Console.Write($"[Line {line}] ");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Write("Warning");
        Console.ResetColor();
        Console.WriteLine($" {where}: {message}");
    }
}