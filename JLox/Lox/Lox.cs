using Lox.Errors;

namespace Lox;

internal static class Lox
{
    private static readonly Interpreter Interpreter = new Interpreter();
    private static readonly Environment Environment = new Environment();
    
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
        var scanner = new Scanner(source);
        var tokens = scanner.ScanTokens();

        var parser = new Parser(tokens);
        var statements = parser.Parse();

        if (hadError) return;

        Interpreter.Interpret(statements!);
    }

    public static void Error(int line, string message)
    {
        Report(line, string.Empty, message);
    }

    public static void Error(Token token, string message)
    {
        if (token.Type == TokenType.EOF)
            Report(token.Line, " at end", message);
        else
            Report(token.Line, $" at '{token.Lexeme}'", message);
    }

    public static void RuntimeError(RuntimeError runtimeError)
    {
        Console.WriteLine($"{runtimeError.Message} \n[line {runtimeError.Token.Line}]");
        hadRuntimeError = true;
    }

    private static void Report(int line, string where, string message)
    {
        Console.WriteLine($"[Line {line}] Error {where}: {message}");
        hadError = true;
    }
}