namespace Tool;

public static class Program
{
    public static void Main(string[] args)
    {
        var tools = new Dictionary<int, Type>
        {
            {1, typeof(GenerateAst)},
            {2, typeof(AstPrinterTest)}
        };

        foreach (var tool in tools)
        {
            Console.WriteLine($"{tool.Key}. {tool.Value.Name}");
        }
        
        var input = Console.ReadLine();
        var num = int.Parse(input!);
        var t = tools[num];
        var ka = (ITool)Activator.CreateInstance(t)!;
        ka.Start(args);
    }
}