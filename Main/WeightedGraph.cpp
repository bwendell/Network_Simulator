#include "WeightedGraph.h"

/**
 * Constructor.
 * 
 * The constructor creates a graph with the maxNumber as the maxSize and
 * allocates memory to the vertexList, adjMatrix and pathMatrix.
 * 
 * @param maxNumber  the max size of the graph
 * 
 * @pre The graph class will be uninitialized.
 * @post The graph will be initialized.
 * 
*/
WeightedGraph::WeightedGraph( int maxNumber )
{
	if( maxNumber > 0 )
	{
		maxSize = maxNumber;
	}
	else
	{
		maxSize = MAX_GRAPH_SIZE;
	}
	
	size = 0;
	
	vertexList = new Vertex[ maxSize ];
	adjMatrix = new int[ maxSize * maxSize ];
	pathMatrix = new int[ maxSize * maxSize ];
	
	for( int i = 0; i < maxSize * maxSize; i++ )
	{
		adjMatrix[ i ] = INFINITE_EDGE_WT;
	}
	
	for( int i = 0; i < maxSize * maxSize; i++ )
	{
		pathMatrix[ i ] = INFINITE_EDGE_WT;
	}	
}

/**
 * Copy constructor.
 * 
 * The copy constructor creates a new graph based on the source graph.
 * 
 * @param WeightedGraph source
 * 
 * @pre The graph will be uninitialized.
 * @post This graph will contain the data in source.
 * 
*/
WeightedGraph::WeightedGraph( const WeightedGraph &source )
{
	maxSize = source.maxSize;
	size = source.size;
	
	vertexList = new Vertex[ maxSize ];
	adjMatrix = new int[ maxSize * maxSize ];
	pathMatrix = new int[ maxSize * maxSize ];
	
	for( int i = 0; i < size; i++ )
	{
		vertexList[ i ] = source.vertexList[ i ];
	}	
	
	for( int i = 0; i < maxSize * maxSize; i++ )
	{
		adjMatrix[ i ] = source.adjMatrix[ i ];
	}
	
	for( int i = 0; i < maxSize * maxSize; i++ )
	{
		pathMatrix[ i ] = source.pathMatrix[ i ];
	}			
}

/**
 * Assignment Operator.
 * 
 * The assignment operator will assign the source graph to this. It first checks 
 * if they are the same object. If they are the same object, this one will
 * be returned. If not, then it will clear this object and copy over
 * all of the data.
 * 
 * @param WeightedGraph source
 * @return Dereference this
 * 
 * @pre This graph won't be the same as source.
 * @post This graph will contain the data in source.
 * 
*/
WeightedGraph& WeightedGraph::operator=( const WeightedGraph &source )
{
	if( this == &source )
	{
		return *this;
	}
	else
	{
		delete[]vertexList;
		delete[]adjMatrix;
		delete[]pathMatrix;
		
		maxSize = source.maxSize;
		size = source.size;
		
		vertexList = new Vertex[ maxSize ];
		adjMatrix = new int[ maxSize * maxSize ];
		pathMatrix = new int[ maxSize * maxSize ];
		
		for( int i = 0; i < size; i++ )
		{
			vertexList[ i ] = source.vertexList[ i ];
		}	
		
		for( int i = 0; i < maxSize * maxSize; i++ )
		{
			adjMatrix[ i ] = source.adjMatrix[ i ];
		}
		
		for( int i = 0; i < maxSize * maxSize; i++ )
		{
			pathMatrix[ i ] = source.pathMatrix[ i ];
		}					
	}
	
	return *this;
}

/**
 * Destructor.
 * 
 * The destructor deallocates all of the memory allocated to the vertexList,
 * adjMatrix and pathMatrix.
 * 
 * @pre The graph won't be deallocated.
 * @post The graph will be deallocated.
 * 
*/
WeightedGraph::~WeightedGraph()
{
	delete[]vertexList;
	delete[]adjMatrix;
	delete[]pathMatrix;
	vertexList = NULL;
	adjMatrix = NULL;
	pathMatrix = NULL;
}

/**
 * insertVertex
 * 
 * The insertVertex function will insert a new vertex into the graph if 
 * the graph isn't full. It will insert it at the end of the vertexList 
 * and adjMatrix. It will then initialize all of the values in the 
 * adjMatrix for this vertex to empty.
 * 
 * @param Vertex newVertex.
 * 
 * @pre The item won't be inserted in the graph.
 * @post The item will be inserted in the graph.
 * 
*/
void WeightedGraph::insertVertex( const Vertex& newVertex ) throw ( logic_error )
{
	if( isFull() )
	{
		throw logic_error( "GRAPH IS FULL!" );
	}
	
	Vertex temp;
	
	if( retrieveVertex( newVertex.getLabel(), temp ) )
	{
		for( int i = 0; i < size; i++ )
		{
			if( vertexList[ i ].getLabel() == newVertex.getLabel() )
			{
				vertexList[ i ] = newVertex;
				break;
			}
		}
	}	
	else
	{
		vertexList[ size ] = newVertex;
		size++;
		
		for( int j = 0; j < maxSize; j++ )
		{
			adjMatrix[ ( ( ( size - 1 ) * maxSize ) + j ) ] = INFINITE_EDGE_WT;
		}		
	}
}

/**
 * insertEdge.
 * 
 * The insertEdge function will insert an edge between two existing 
 * vertexes. If the two vertexes don't exist, it will throw a logic error.
 * It will then insert an edge between the two denoting the weight
 * between the two of them.
 * 
 * @param string v1  
 * @param string v2
 * @param int wt  
 * 
 * @pre There will be no edge between two vertexes.
 * @post There will be an edge between two vertexes.
 * 
*/
void WeightedGraph::insertEdge( const string& v1, const string& v2, int wt ) throw ( logic_error )
{
	Vertex temp;
	
	if( !retrieveVertex( v1, temp ) || !retrieveVertex( v2, temp ) )
	{
		throw logic_error( "VERTEX NOT IN GRAPH!" );		
	}
	
	if( v1 != v2 )
	{
		int v1Location, v2Location;
		v1Location = getIndex( v1 );
		v2Location = getIndex( v2 );
		
		setEdge( v1Location, v2Location, wt );
		setEdge( v2Location, v1Location, wt );
	}		
}

/**
 * retrieveVertex.
 * 
 * This function will loop through the vertexList to find the vertex
 * that is being looked for. If it is found, that vertex will be copied 
 * to vData and the function will return true.
 * 
 * @param string v  
 * @param Vertex vData
 * 
 * @pre The item isn't retrieved.
 * @post This item is retrieved.
 * 
 * @return This has a bool return.
 * 
*/
bool WeightedGraph::retrieveVertex( const string& v, Vertex& vData ) const
{
	for( int i = 0; i < size; i++ )
	{
		if( vertexList[ i ].getLabel() == v )
		{
			vData = vertexList[ i ];
			return true;
		}
	}
	
	return false;
}

/**
 * getEdgeWeight.
 * 
 * This function checks to see if the two vertexes are contained within 
 * the graph. If they are then it gets the edge weight between them.
 * 
 * @param string v1 
 * @param string v2 
 * @param int wt  
 * @return This has a bool return.
 * 
 * @pre The weight isn't found
 * @post The weight is found.
 * 
*/
bool WeightedGraph::getEdgeWeight( const string& v1, const string& v2, int& wt ) const throw ( logic_error )
{
	Vertex temp;
	
	if( !retrieveVertex( v1, temp ) || !retrieveVertex( v2, temp ) )
	{
		throw logic_error( "VERTEX NOT IN GRAPH!" );
	}
	
	int v1Location, v2Location;
	
	v1Location = getIndex( v1 );
	v2Location = getIndex( v2 );
		
	wt = getEdge( v1Location, v2Location );
	
	if( wt == INFINITE_EDGE_WT )
	{
		return false;
	}
	
	return true;
}

/**
 * removeVertex.
 * 
 * This function checks to see if the vertex is contained within the 
 * vertexList.  If it is then it is removed from the list and the vertex
 * list is arragned accordingly.  The ajdMatrix is also adjusted accordingly.
 * 
 * @param string v
 * 
 * @pre The vertex won't be deleted.
 * @post The vertex will be deleted if it exits.
 * 
*/
void WeightedGraph::removeVertex( const string& v ) throw ( logic_error )
{
	Vertex temp;
	int vLocation = getIndex( v );
	
	if( !retrieveVertex( v, temp ) )
	{
		throw logic_error( "VERTEX NOT IN GRAPH!" );
	}
	
	for( int i = vLocation; i < size; i++ )
	{
		vertexList[ i ] = vertexList[ i + 1 ];
	}
	
	for( int i = vLocation; i < size; i++ )
	{
		 for( int j = 0; j < maxSize; j++ )
		 {
			 adjMatrix[ ( ( i * maxSize ) + j ) ] =
			 adjMatrix[ ( (  ( i + 1 ) * maxSize ) + j ) ];
		 }
	}
	
	for( int i = 0; i < size; i++ )
	{
		 for( int j = vLocation; j < maxSize; j++ )
		 {
			 adjMatrix[ ( ( i * maxSize ) + j ) ] =
			 adjMatrix[ ( ( i * maxSize ) + j + 1 ) ];
		 }
	}			
	
	size--;		
}

/**
 * removeEdge.
 * 
 * This function checks to see if the two vertexes exists.  If they do
 * then it removes the edge between them.
 * 
 * @param string v1
 * @param string v2
 * 
 * @pre The edge isn't removed.
 * @post The edge is removed.
 * 
*/
void WeightedGraph::removeEdge( const string& v1, const string& v2 ) throw ( logic_error )
{
	Vertex temp;
	
	if( !retrieveVertex( v1, temp ) || !retrieveVertex( v2, temp ) )
	{
		throw logic_error( "VERTEX NOT IN GRAPH!" );
	}
		
	int v1Location, v2Location;
	
	v1Location = getIndex( v1 );
	v2Location = getIndex( v2 );
		
	setEdge( v1Location, v2Location, INFINITE_EDGE_WT );
	setEdge( v2Location, v1Location, INFINITE_EDGE_WT );
}

/**
 * clear.
 * 
 * This function clears the graph and sets the size equal to zero.
 *  
 * @pre This graph isn't cleared.
 * @post This graph is cleared.
 * 
*/
void WeightedGraph::clear()
{
	for( int i = 0; i < maxSize * maxSize; i++ )
	{
		adjMatrix[ i ] = INFINITE_EDGE_WT;
	}
	
	size = 0;
}

/**
 * isEmpty
 * 
 * This function checks to see if the size is 0. 
 * 
 * @return This returns a bool.
 * 
 * @pre It will be unknown if the graph is empty.
 * @post It will be known if the graph is empty.
 * 
*/
bool WeightedGraph::isEmpty() const
{
	return ( size == 0 );
}

/**
 * isFull.
 * 
 * This function will see if the size is equal to maxSize.
 * 
 * @return This returns a bool.
 * 
 * @pre It will be unknown if the graph is full.
 * @post It will be known if the graph is full.
 * 
*/
bool WeightedGraph::isFull() const
{
	return( size == maxSize );
}

/**
 * getIndex
 * 
 * This function checks to to see if the vertex is in the graph. If it is 
 * in the graph, it will return that index. If it isn't in the graph, 
 * it will return a -99999.
 * 
 * @param string v 
 * @return int index is return.
 * 
 * @pre The vertex's index is unknown.
 * @post The vertex's index is known.
 * 
*/
int WeightedGraph::getIndex( const string &v ) const
{
	int index = -99999;
	
	for( int i = 0; i < size; i++ )
	{
		if( vertexList[ i ].getLabel() == v )
		{
			index = i;
			return index;
		}		
	}
	
	return index;
}

/**
 * getEdge.
 * 
 * This function finds the edge given the row and col of the adjMatrix
 * 
 * @param int row
 * @param int col 
 * 
 * @pre The edge is unknown.
 * @post The edge is known.
 * 
*/
int WeightedGraph::getEdge( int row, int col ) const
{
	int edge;
	
	if( !isEmpty() )
	{
		edge = adjMatrix[ ( ( row * maxSize ) + col ) ];
	}
	
	return edge;
}

/**
 * setEdge.
 * 
 * This function simply sets the edge given the row and col of the 
 * adjMatrix and the weight.
 * 
 * @param int row 
 * @param int col  
 * @param int wt
 * 
 * @pre The edge won't be set.
 * @post The edge will be set.
 * 
*/
void WeightedGraph::setEdge ( int row, int col, int wt)
{
	if( !isEmpty() )
	{
		adjMatrix[ ( ( row * maxSize ) + col ) ] = wt;
	}
}

/**
 * showShortestPaths.
 * 
 * This function finds the shortest path and updates as well as dispalys
 * the pathMatrix.  
 * 
 * @pre The path matrix won't be shown.
 * @post The path matrix will be shown.
 * 
*/
void WeightedGraph::showShortestPaths () 
{	
	for( int p = 0; p < ( maxSize * maxSize ); p++ )
		pathMatrix[ p ] = adjMatrix[ p ];
		
	for( int i = 0; i < size; i++ )
		pathMatrix[ ( i * maxSize ) + i ] = 0;
	
	for( int m = 0; m < size; ++m )
	{
		for( int j = 0; j < size; ++j )
		{
			for( int k = 0; k < size; ++k )
			{
				// check if there is a path between j and k through m
				if( ( pathMatrix[ ( j * maxSize ) + m ]  > 0 && 
						pathMatrix[ ( j * maxSize ) + m ] < INT_MAX ) && 
						( pathMatrix[ ( m * maxSize ) + k ] > 0 && 
							pathMatrix[ ( m * maxSize ) + k ] < INT_MAX )
							&& ( j != k && j != m && m != k ) )
				{
					// check to see if path is shorter than direct path
					if( ( pathMatrix[ ( j * maxSize ) + m ] + pathMatrix[ ( m * maxSize ) + k ] )
							< pathMatrix[ ( j * maxSize ) + k ] ) 
					{
						pathMatrix[ ( j * maxSize ) + k ] = 
							( pathMatrix[ ( j * maxSize ) + m ] +
							pathMatrix[ ( m * maxSize ) + k ] );
					}
				}
			}
		}
	}
		
	if ( size != 0 ) 
	{
       cout << endl << "Shortest Path matrix : " << endl << '\t';
       
       for ( int col = 0 ; col < size ; col++ )
           cout << col << '\t'; 
       cout << endl;    
       for ( int row = 0 ; row < size ; row++ )
       {
           cout << row << '\t';
           for ( int col = 0 ; col < size ; col++ )
           {
			   int wt = pathMatrix[ ( ( row * maxSize ) + col ) ];
               if ( wt == INFINITE_EDGE_WT )
                  cout << "- \t";
               else
                  cout << wt << '\t';
           }
           cout << endl;
       }
    }
    else
		cout << endl << "Path matrix : " << endl << endl;
}


