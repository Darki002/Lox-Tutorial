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
            "Binary   : Expr Left, Token Operator, Expr Right",
            "Grouping : Expr Expression",
            "Literal  : object? Value",
            "Unary    : Token Operator, Expr Right"
        ]);
    }

    private static void DefineAst(string outputDir, string baseName, List<string> types)
    {
        var path = $"{outputDir}/{baseName}.cs";
        using var writer = new StreamWriter(path, false, System.Text.Encoding.UTF8);
        
        writer.WriteLine("namespace Lox;");
        writer.WriteLine();
        writer.WriteLine($"public abstract record {baseName}");
        writer.WriteLine("{");
        
        foreach (var type in types) {
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
        
        foreach (var type in types) {
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