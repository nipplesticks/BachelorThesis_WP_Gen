#define EPSILON 0.0001f

struct Triangle
{
	float3 Position[3];
	float3 Normal;
};

struct TreeNode
{
	uint ByteStart;

	float2 Min;
	float2 Max;

	uint NrOfChildren;
	uint ChildrenByteAddress[4];

	uint NrOfTriangles;
};

struct AddressStack
{
	uint Address;
	uint TargetChildren;
};

struct Waypoint
{
	uint	Key;
	uint	NrOfConnections;
	float2	Pos;
	uint	Connections[256];
};

cbuffer OffsetBuffer : register(b0)
{
    uint4 OFFSET;
}
StructuredBuffer<Triangle> Triangles : register(t0);		// Triangles
ByteAddressBuffer QuadTreeBuffer : register(t1);			// Quad Tree
RWStructuredBuffer<Waypoint> Waypoints : register(u0);		// Waypoints

Triangle GetTriangle(in uint address, in uint index)
{
    uint triIndex = QuadTreeBuffer.Load(address + index * 4);
    return Triangles[triIndex];
}

TreeNode GetNode(in uint address, out uint triangleIndexAddress)
{
	TreeNode node = (TreeNode)0;

	address += 4; // byteSize

	node.ByteStart = QuadTreeBuffer.Load(address);
	address += 4; // byteStart

	node.Min = asfloat(QuadTreeBuffer.Load2(address));
	address += 8; // min

	node.Max = asfloat(QuadTreeBuffer.Load2(address));
	address += 8; // max

	address += 4; // Level

	node.NrOfChildren = QuadTreeBuffer.Load(address);
	address += 4; // nrOfChildren

	address += 4 * 4; // childrenIndices
	
    [unroll]
    for (uint j = 0; j < 4; j++)
	{
		node.ChildrenByteAddress[j] = QuadTreeBuffer.Load(address);
		address += 4; // childrenByteAdress[i]
	}

	node.NrOfTriangles = QuadTreeBuffer.Load(address);
	address += 4; // numberOfObjects

	triangleIndexAddress = address;

	return node;
}

bool LineLineIntersect(float2 l1Origin, float2 l1End, float2 l2Origin, float2 l2End)
{
    float2 b = l1End - l1Origin;
    float2 d = l2End - l2Origin;

    float bDotDPerp = b.x * d.y - b.y * d.x;

    if (abs(bDotDPerp) < EPSILON)
        return false;

    float2 c = l2Origin - l1Origin;

    float t = (c.x * d.y - c.y * d.x) / bDotDPerp;

    if (t < 0.0f || t > 1.0f)
        return false;

    float u = (c.x * b.y - c.y * b.x) / bDotDPerp;

    if (u < 0.0f || u > 1.0f)
        return false;
	
    return true;
}

bool LineQuadIntersect(float2 origin, float2 end, float2 Min, float2 Max)
{
    float2 topRight, bottomLeft;
    topRight.x = Max.x;
    topRight.y = Min.y;
    bottomLeft.x = Min.x;
    bottomLeft.y = Max.y;

    if (origin.x >= Min.x && origin.x <= Max.x &&
        origin.y >= Min.y && origin.y <= Max.y)
        return true;

    if (end.x >= Min.x && end.y <= Max.x &&
        end.y >= Min.y && end.y <= Max.y)
        return true;
    
    if (LineLineIntersect(origin, end, Min, topRight))
        return true;
    if (LineLineIntersect(origin, end, topRight, Max))
        return true;
    if (LineLineIntersect(origin, end, Max, bottomLeft))
        return true;
    if (LineLineIntersect(origin, end, bottomLeft, Min))
        return true;
	
    return false;
}

bool PointTriangleIntersect(float2 s, float2 a, float2 b, float2 c)
{
    float as_x = s.x - a.x;
    float as_y = s.y - a.y;

    bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

    if ((c.x - a.x) * as_y - (c.y - a.y) * as_x >= 0 == s_ab)
        return false;

    if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0 != s_ab)
        return false;

    return true;
}

bool LineTriangleIntersect(float2 origin, float2 end, Triangle tri)
{
    float2 p0, p1, p2;
    p0.x = tri.Position[0].x;
    p0.y = tri.Position[0].z;

    p1.x = tri.Position[1].x;
    p1.y = tri.Position[1].z;

    p2.x = tri.Position[2].x;
    p2.y = tri.Position[2].z;
    
    if (PointTriangleIntersect(origin, p0, p1, p2))
        return true;
    if (PointTriangleIntersect(end, p0, p1, p2))
        return true;
    if (LineLineIntersect(origin, end, p0, p1))
        return true;
    if (LineLineIntersect(origin, end, p1, p2))
        return true;
    if (LineLineIntersect(origin, end, p2, p0))
        return true;

    return false;
}

static const uint WP_SPLIT = 1024;

[numthreads(WP_SPLIT, 1, 1)]
void main(uint3 threadID : SV_GroupID, uint3 threadGroup : SV_GroupThreadID)
{
    uint waypointTarget = threadID.x + OFFSET.x;
    uint wpSplit = threadGroup.x;

    Waypoint target = Waypoints[waypointTarget];
    uint nrOfWaypoints = 0, dummy = 0;
    Waypoints.GetDimensions(nrOfWaypoints, dummy);
    
    uint numberOfWP = (float)nrOfWaypoints / WP_SPLIT + 0.5f;
    uint wpStart = numberOfWP * wpSplit;
    uint wpEnd = wpStart + numberOfWP;

    if (wpSplit == WP_SPLIT - 1)
        wpEnd = nrOfWaypoints;

    wpEnd = min(wpEnd, nrOfWaypoints);

    float2 origin = target.Pos;
    
    for (uint i = wpStart; i < wpEnd; i++)
    {
        if (i == waypointTarget)
            continue;
        bool hit = false;
        
        Waypoint towards = Waypoints[i];
        float2 end = towards.Pos;
        
        AddressStack nodeStack[16];
        uint nodeStackSize = 0;
        uint triIndexAddress = 0;
        TreeNode node = GetNode(0, triIndexAddress);
        
        if (LineQuadIntersect(origin, end, node.Min, node.Max))
        {
            bool intersection = false;

            nodeStack[nodeStackSize].Address = node.ByteStart;
            nodeStack[nodeStackSize].TargetChildren = 0;
            nodeStackSize++;
			
            while (nodeStackSize > 0 && !intersection)
            {
                uint currentNode = nodeStackSize - 1;
                node = GetNode(nodeStack[currentNode].Address, triIndexAddress);
                if (nodeStack[currentNode].TargetChildren < node.NrOfChildren)
                {
                    TreeNode child = GetNode(node.ChildrenByteAddress[nodeStack[currentNode].TargetChildren++], triIndexAddress);
                    if (LineQuadIntersect(origin, end, child.Min, child.Max))
                    {
                        if (child.NrOfTriangles > 0)
                        {
                            for (uint triIt = 0; triIt < child.NrOfTriangles && !intersection; triIt++)
                            {
                                Triangle tri = GetTriangle(triIndexAddress, triIt);
                                intersection = LineTriangleIntersect(origin, end, tri);
                                if (intersection)
                                    hit = true;
                            }
                        }
                        else
                        {
                            nodeStack[nodeStackSize].Address = child.ByteStart;
                            nodeStack[nodeStackSize].TargetChildren = 0;
                            nodeStackSize++;
                        }

                    }
                }
                else
                {
                    nodeStackSize--;
                }

            } // While End

            if (!hit)
            {
                if (WP_SPLIT > 1)
                {
                    uint index;
                    InterlockedAdd(Waypoints[waypointTarget].NrOfConnections, 1, index);
                    index = min(index, 255);
                    Waypoints[waypointTarget].Connections[index] = towards.Key;
                }
                else
                {
                    uint index = Waypoints[waypointTarget].NrOfConnections;
                    Waypoints[waypointTarget].Connections[index] = towards.Key;
                    Waypoints[waypointTarget].NrOfConnections++;
                }
            }
			
        } // If hit master node End
    } // For end
}