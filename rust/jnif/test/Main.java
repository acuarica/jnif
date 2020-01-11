import static java.lang.System.out;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Map.Entry;

class Node {
    public final List<Edge> edges = new ArrayList<Edge>();
    public String value;

    public Node(String value) {
        this.value = value;
    }
}

class Edge {
    public Node node;
    public int weight;

    public Edge(Node node, int weight) {
        this.node = node;
        this.weight = weight;
    }
}

public class Main {

    static void print(Node node, List<Node> visited) {
        if (visited.contains(node)) {
            return;
        }

        visited.add(node);
        out.println(node.value);
        for (Edge edge : node.edges) {
            out.print("-> ");
            out.print(edge.weight);
            print(edge.node, visited);
        }
    }

    static Map<Node, Integer> sp(List<Node> graph, Node s) {
        Map<Node, Integer> d = new HashMap<Node, Integer>();
        List<Node> visited = new ArrayList<Node>();

        // Long.MAX_VALUE
        // 9,223,372,036,854,775,807
        for (Node n : graph) {
            d.put(n, Integer.MAX_VALUE);
        }
        d.put(s, 0);

        Queue<Node> q = new ArrayDeque<Node>();
        q.add(s);
        while (!q.isEmpty()) {
            Node node = q.poll();
            visited.add(node);

            for (Edge e : node.edges) {
                if (!visited.contains(e.node)) {
                    q.add(e.node);
                }
                if (d.get(e.node) > d.get(node) + e.weight) {
                    d.put(e.node, d.get(node) + e.weight);
                }
            }
        }

        return d;
    }

    public static boolean solution(int[] A) {
        // a = b = c: return true
        // a = b < c, a > b = c: return false
        // a < b > c, a = c: move both inwards
        // a > b < c, a = c: return false
        // a = b > c, a < b = c: move both inwards
        // a < b < c, a > b > c: &a++
        int i = 1;
        int j = A.length - 2;

        int a = A[i-1];
        int b = 0;
        for (int x = i+1; x<j; x++ ) {
            b += A[x];
        }
        int c = A[j+1];

        while () {

        }

        return true;
    }

    public static void main2(final String[] args) {
        int x = 100000;
        int y = 100000;
        out.println(x * y);
        // out.println("Main");
        // Node a = new Node("a");
        // Node b = new Node("b");
        // Node c = new Node("c");
        // Node d = new Node("d");
        // Node e = new Node("e");
        // a.edges.add(new Edge(b, 1));
        // a.edges.add(new Edge(c, 4));
        // b.edges.add(new Edge(c, 2));
        // b.edges.add(new Edge(d, 3));
        // d.edges.add(new Edge(e, 3));
        // e.edges.add(new Edge(b, 3));

        // List<Node> visited = new ArrayList<Node>();
        // print(a, visited);

        // out.println("");

        // Map<Node, Integer> ds = sp(visited, a);
        // for (Entry<Node, Integer> entry : ds.entrySet()) {
        // out.print(entry.getKey().value);
        // out.println(entry.getValue());
        // }
    }
}
