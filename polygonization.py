
class Graph_Alg:
    @staticmethod
    def convex_hull(point_set):
        hull = []
        point_set.sort(key=lambda v:v.x)
        p, q, ln = 0, 0, len(point_set)

        while True:
            hull += [point_set[p]]
            q = (p + 1) % ln
            for i in range(0, len(point_set)): 
                if (Graph_Alg.orientation(point_set[p], point_set[i], point_set[q]) == 2):
                    q = i
            p = q
            if p == 0:
                break
        
        return point_set

    @staticmethod
    def orientation(p, q, r):
        val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y) 
        if (val == 0):
             return 0 # colinear 
        return 1 if (val > 0) else 2 # clock or counterclock wise 

    @staticmethod
    def peel(point_set):
        clis = point_set[:]
        peel = []
        while len(clis) > 0:
            ch = Graph_Alg.convex_hull(clis)
            peel += [ch]
            clis = [x for x in clis if x not in ch]
        return peel

    @staticmethod
    def calculate_area(point_set):
        point_set.sort(key=lambda v:v.x)
        area = 0.0
        size = len(point_set)
        for i in range(0, size):
            j = (i + 1) % size
            area += point_set[i].x * point_set[j].y
            area -= point_set[i].y * point_set[j].x
        return abs(area)/2


    @staticmethod
    def capture(point_set):
        pass

class Graph:

    adj_list = None

    def __init__(self):
        self.adj_list = {}

    def add_vertex(self, v):
        if v not in self.adj_list:
            self.adj_list[v] = []

    def add_edge(self, v1, v2):
        if v1 not in self.adj_list:
            self.adj_list[v1] = []

        if v2 not in self.adj_list:
            self.adj_list[v2] = []

        self.adj_list[v1] += [v2]
        self.adj_list[v2] += [v1]

    def point_set(self):
        return self.adj_list.keys()

class Vertex:
    x = None
    y = None

    def __init__(self, x, y):
        self.x = x
        self.y = y


class File_Processor:
    @staticmethod
    def file_to_graph(graph, filepath):
        with open(filepath) as file:
            for _ in range(2):
                next(file)
            for line in file:
                vertex = File_Processor.line_to_vertex(line)
                graph.add_vertex(vertex)

    @staticmethod
    def line_to_vertex(line):
        split_line = line.split()[1:] # Swallow the vertex number
        return Vertex(int(split_line[0]), int(split_line[1]))

    
class Instance:

    graph = None
    filepath  = None

    def __init__(self, filepath):
        self.graph = Graph()
        self.file = filepath
        File_Processor.file_to_graph(self.graph, filepath)
        hull = Graph_Alg.convex_hull(self.graph.point_set())
        Graph_Alg.peel(self.graph.point_set())
        print(Graph_Alg.calculate_area(hull))


i = Instance('/Users/ehudadler/Downloads/challenge_instances/data/images/euro-night-0000010.instance')


