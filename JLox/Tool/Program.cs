namespace Tool;

public static class Program
{
    public static void Main(string[] args)
    {
        var generator = new GenerateAst();
        generator.Start(args);
    }
}