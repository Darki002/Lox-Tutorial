using System.Text;

namespace Tool;

public class GenerateAst : ITool
{
    public void Start(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: generate_ast <output directory>");
            Environment.Exit(64);
        }

        var outputDir = args[0];

        DefineAst(outputDir, "Expr", [
            "Assign   : Token Name, Expr Value",
            "Binary   : Expr Left, Token Operator, Expr Right",
            "Grouping : Expr Expression",
            "Literal  : object? Value",
            "Logical  : Expr Left, Token Operator, Expr Right",
            "Set      : Expr Obj, Token Name, Expr Value",
            "IndexSet : Expr Obj, Expr Index, Expr Value, Token Token",
            "Super    : Token Keyword, Token Method",
            "This     : Token Keyword",
            "Unary    : Token Operator, Expr Right",
            "Call     : Expr Callee, Token Paren, List<Expr> Arguments",
            "Get      : Expr Obj, Token Name",
            "IndexGet : Expr Obj, Expr Index, Token Token",
            "Variable : Token Name",
            "Function : List<Token> Params, List<Stmt> Body"
        ]);

        DefineAst(outputDir, "Stmt", [
            "Block      : List<Stmt> Statements",
            "Class      : Token Name, Expr.Variable? Superclass, List<Stmt.Function> Methods, List<Stmt.Function> ClassMethods",
            "Expression : Expr Body",
            "Function   : Token Name, List<Token>? Params, List<Stmt> Body",
            "If         : Expr Condition, Stmt ThenBranch, Stmt? ElseBranch",
            "Print      : Expr Right",
            "Return     : Token Keyword, Expr? Value",
            "Var        : Token Name, Expr? Initializer",
            "While      : Expr Condition, Stmt Body",
            "Break      : Token Keyword"
        ]);
    }

    private static void DefineAst(string outputDir, string baseName, List<string> types)
    {
        var path = $"{outputDir}/{baseName}.cs";
        using var writer = new StreamWriter(path, false, Encoding.UTF8);

        writer.WriteLine("// ReSharper disable once CheckNamespace");
        writer.WriteLine("namespace Lox;");
        writer.WriteLine();
        writer.WriteLine($"public abstract record {baseName}");
        writer.WriteLine("{");

        foreach (var type in types)
        {
            var className = type.Split(":")[0].Trim();
            var fields = type.Split(":")[1].Trim();
            DefineType(writer, baseName, className, fields);
            writer.WriteLine();
        }

        writer.WriteLine("\tpublic abstract T Accept<T>(IVisitor<T> visitor);");

        DefineVisitor(writer, baseName, types);

        writer.WriteLine("}");
        writer.Close();
    }

    private static void DefineVisitor(StreamWriter writer, string baseName, List<string> types)
    {
        writer.WriteLine();
        writer.WriteLine("\tpublic interface IVisitor<out T>");
        writer.WriteLine("\t{");

        foreach (var type in types)
        {
            var typeName = type.Split(":")[0].Trim();
            writer.WriteLine($"\t\tT Visit{typeName}{baseName} ({typeName} {baseName.ToLower()});");
        }

        writer.WriteLine("\t}");
    }

    private static void DefineType(StreamWriter writer, string baseName, string className, string fields)
    {
        writer.WriteLine($"\tpublic record {className}({fields}) : {baseName}");
        writer.WriteLine("\t{");

        writer.WriteLine("\t\tpublic override T Accept<T>(IVisitor<T> visitor)");
        writer.WriteLine("\t\t{");

        writer.WriteLine($"\t\t\treturn visitor.Visit{className}{baseName}(this);");

        writer.WriteLine("\t\t}");

        writer.WriteLine("\t}");
    }
}