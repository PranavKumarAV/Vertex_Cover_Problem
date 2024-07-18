#!/usr/bin/env python3
import sys

def line_intersection(p1_src, p1_dst, p2_src, p2_dst):
    x1, y1 = p1_src[0], p1_src[1]
    x2, y2 = p1_dst[0], p1_dst[1]
    x3, y3 = p2_src[0], p2_src[1]
    x4, y4 = p2_dst[0], p2_dst[1]

    # Calculate the slopes of the two lines
    m1 = (y2 - y1) / (x2 - x1) if x2 - x1 != 0 else float('inf')
    m2 = (y4 - y3) / (x4 - x3) if x4 - x3 != 0 else float('inf')

    # Check if the lines are parallel
    if m1 == m2:
        return None

    # Calculate the intersection point
    if m1 == float('inf'):
        x = x1
        y = m2 * (x - x3) + y3
    elif m2 == float('inf'):
        x = x3
        y = m1 * (x - x1) + y1
    else:
        x = (y3 - y1 + m1 * x1 - m2 * x3) / (m1 - m2)
        y = m1 * (x - x1) + y1


    # Check if the intersection point is within the line segments
    if (
        min(x1, x2) - 0.01 <= x <= max(x1, x2) + 0.01 and
        min(x3, x4) - 0.01 <= x <= max(x3, x4) + 0.01 and
        min(y1, y2) - 0.01 <= y <= max(y1, y2) + 0.01 and
        min(y3, y4) - 0.01 <= y <= max(y3, y4) + 0.01
    ):
        return [x, y]

    return None


def parseLine(line_original):
    """Parse an input line and retrun command and argument

       Throws an exception on error.
    """
    command = []        
    #Identify command
    line = line_original.strip('\n')            #Removes \n
    line = line.split('"', maxsplit = 1)        #Seperate command from street name and co-ordinates
    if line[0][-1] != " " and "gg" not in line[0]:
        raise Exception('There must be space between commands. Please try again.')
    # for i in range(len(line)):
    #     line[i] = line[i].replace(" ","")       #Removes space between all the words
    line[0] = line[0].replace(" ","")
    if line[0] not in ['add', 'mod', 'rm', 'gg']:
        raise Exception('Unknown command. Please try again.')
    command.append(line[0].lower())

    if command[0] == 'gg':
        if len(line) > 1:
            raise Exception('Invalid input format. "gg" is not followed by any characters. Please try again')
        
    elif command[0] == 'rm':
        if line[1][-1] == "\"":                     #Checks if correctly enclosed
            line[1]=line[1].replace('"','')         #Removes " from the street name
        elif "\"" in line[-1]:
            raise Exception('Invalid input format. Remove details other than street name. Please try again.')
        else:
            raise Exception('Invalid input format. Enclose Street name in " ". Please try again.')
        line = line[1].split('"', maxsplit = 1) 
        command.append(line[0])
        if len(line)>1:
            raise Exception("rm does not take co-ordinate inputs. Please try again")

    else:
        #Collect street name
        try:
            line = line[1].split('"', maxsplit = 1) 
        except:
            raise Exception('Invalid input format. Enclose Street name in " ". Please try again.')

        if line[1][0] != " ":
            raise Exception('Invalid input format. Provide space between street name and coordinates. Please try again.')
        
        command.append(line[0])

        #Collect co-ordinates
        try:
            line=line[1].replace(" ","")
        except:
            raise Exception("Co-ordinates required. Please try again")

        #Co-Ordinate format checking. Even though several while loops are nested, it parses the line only once so it is O(n) not quadratic
        index=0
        check = 0
        co_ord = list(line)
        exception_coord = "Invalid co-ordinate entry. Check if the entry is of the format (a,b)(c,d). Please try again"
        while(index<len(co_ord)):
            if co_ord[index] == '(':
                index+=1
                while(co_ord[index] in ['0','1','2','3','4','5','6','7','8','9','-']):
                    index+=1
                    check = 1
                if co_ord[index]==',' and (check == 1):
                    index+=1
                    while(co_ord[index] in ['0','1','2','3','4','5','6','7','8','9','-']):
                        index+=1
                        check = 2
                    if co_ord[index]==')' and (check == 2):
                        index+=1
                        check = 0
                    else:
                        raise Exception(exception_coord)
                else:
                    raise Exception(exception_coord)
            else:
                raise Exception(exception_coord)


        line = line.replace(')(',',')
        line = line.replace('(','')
        line = line.replace(')','')
        line_list = list(line.split(','))
        try:
            line_list_coor = [float(i) for i in line_list]
        except:
            raise Exception("Invalid format for co-ordinates. Please try again.")
        if len(line_list_coor)%2 == 1:
            raise Exception("Invalid co-ordinates entry. Please try again")
        
        if len(line_list_coor) < 3:
            raise Exception("Enter more than one coordinates. Please try again")
        
        if command[1] == "":
            raise Exception("Space name should be valid. Please try again")


        #Forming street
        i=0
        line_list = []

        
        for i in range(0,len(line_list_coor)-1,2):
            if (line_list_coor[i]>99999) or (line_list_coor[i+1]>99999):
                raise Exception('Co-ordinate value out of range. Please try again')
            line_list.append([line_list_coor[i],line_list_coor[i+1]])
        command.append(line_list)  
    return command

def add_street(streets, line):

    if line[1] in streets.keys():
        raise Exception('Invalid Operation. Addition failed. Street already exists in database. Please try again')

    streets[line[1]] = line[2]

    return

def mod_street(streets, line):

    if line[1] not in streets.keys():
        raise Exception('Invalid Operation. modification failed. Street does not exist in database. Please try again')

    streets[line[1]] = line[2]

    return

def rm_street(streets, line):

    if line[1] not in list(streets.keys()):
        raise Exception('Invalid Operation. deletion failed. Street does not exist in database. Please try again')
    
    streets.pop(line[1])

    return

def check_fn(streets, intersections):
    for s1 in streets:
        for p in streets[s1]:
            for street in streets:
                if s1 != street:
                    if p in streets[street] and p not in intersections:
                        intersections.append(p)
    
    return intersections


def gg(streets):
    vertices = []
    edges = []
    intersections = []
    inter_st = {}

    for one_street in streets:
        for i in range(len(streets[one_street])-1):
            if [streets[one_street][i], streets[one_street][i+1]] in edges or [streets[one_street][i+1], streets[one_street][i]] in edges:
                continue
            else:
                edges.append([streets[one_street][i], streets[one_street][i+1]])
        inter_st[one_street] = []


    if len(edges) > 1:
        adjust_outer = 0
        intersections = []
        completed = False
        while completed == False:
            for i in range(adjust_outer, len(edges)-1):
                temp_intersections = []
                adjust_inner = 0
                for j in range(i+1,len(edges)):
                    if edges[i][0] in edges[j + adjust_inner] or edges[i][1] in edges[j + adjust_inner]:
                        continue
                    else:
                        inter = line_intersection(edges[i][0], edges[i][1], edges[j + adjust_inner][0], edges[j + adjust_inner][1])
                        if inter:
                            if inter not in edges[j + adjust_inner]: 
                                edges.insert(j + adjust_inner + 1,[inter,edges[j+adjust_inner][1]])
                                edges[j+adjust_inner][1] = inter
                                adjust_inner += 1
                            if inter not in temp_intersections:
                                temp_intersections.append(inter)
                if len(temp_intersections) != 0:
                    adjust_temp = 0
                    for pt_inter in temp_intersections:
                        x, y = pt_inter[0], pt_inter[1]
                        for pt_temp in range(i,i+adjust_temp+1):
                            if pt_inter not in edges[pt_temp]:
                                x1, y1 = edges[pt_temp][0][0], edges[pt_temp][0][1]
                                x2, y2 = edges[pt_temp][1][0], edges[pt_temp][1][1]
                                if min(x1, x2) <= x <= max(x1, x2) and min(y1, y2) <= y <= max(y1, y2):
                                    edges.insert(pt_temp+1,[pt_inter,edges[pt_temp][1]])
                                    edges[pt_temp][1] = pt_inter
                                    adjust_temp += 1
                                    break
                    adjust_outer += adjust_temp + 1
                    for pt_inter in temp_intersections:
                        if pt_inter not in intersections:
                            intersections.append(pt_inter)
                if i < len(edges)-2:
                    if len(temp_intersections) == 0:
                        adjust_outer += 1
                    break
                else:
                    completed = True

            if adjust_outer - 1 == len(edges) - 2:
                completed = True

    intersections = check_fn(streets, intersections)

    ###find vertex and edges
    vertices.extend(intersections)

    adjust_final = 0
    for i in range(len(edges)):
        if edges[i-adjust_final][0] in intersections and edges[i-adjust_final][1] in intersections:
            continue
        elif edges[i-adjust_final][0] in intersections and edges[i-adjust_final][1] not in vertices:
            vertices.append(edges[i-adjust_final][1])
        elif edges[i-adjust_final][1] in intersections and edges[i-adjust_final][0] not in vertices:
            vertices.append(edges[i-adjust_final][0])
        elif edges[i-adjust_final][0] not in intersections and edges[i-adjust_final][1] not in intersections:
            edges.remove(edges[i-adjust_final])
            adjust_final += 1


    ###making final changes to edges: naming different

    for i in range(len(edges)):
        edges[i][0], edges[i][1] = vertices.index(edges[i][0])+1, vertices.index(edges[i][1])+1


    ###print vertices and edges
    print("V = {",file=sys.stdout)
    for i in range(len(vertices)):
        print(" ",str(i+1)+":"," ("+str(round(vertices[i][0],2))+","+str(round(vertices[i][1],2))+")",file=sys.stdout)
    print("}",file=sys.stdout)

    print("E = {",file=sys.stdout)
    for i in range(len(edges)):
        if i == len(edges) - 1:
            print(" ","<"+str(edges[i][0])+","+str(edges[i][1])+">",file=sys.stdout)
            break
        print(" ","<"+str(edges[i][0])+","+str(edges[i][1])+">,",file=sys.stdout)
    print("}",file=sys.stdout)


def main():
    # YOUR MAIN CODE GOES HERE

    streets = {}

    while True:
        #Reading and evaluating input
        #line = ['null']
        line = sys.stdin.readline()

        if line == "":      
            break
        try:
            line = parseLine(line)
        except Exception as e:
            print("Error: Invalid Input: "+str(e), file=sys.stderr) 

        if line[0]=='add':
            try:
                add_street(streets,line)
            except Exception as e:
                print("Error: Invalid Operation: "+str(e), file=sys.stderr) 

        elif line[0]=='mod':
            try:
                mod_street(streets,line)
            except Exception as e:
                print("Error: Invalid Operation: "+str(e), file=sys.stderr) 

        elif line[0]=='rm':
            try:
                rm_street(streets,line)
            except Exception as e:
                print("Error: Invalid Operation: "+str(e), file=sys.stderr) 

        elif line[0]=='gg':
            try:
                gg(streets)
            except Exception as e:
                print("Error: Operation Error: "+str(e), file=sys.stderr) 

    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == "__main__":
    main()
