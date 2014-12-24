#ifndef WEIGHTEDGRAPH_H
#define WEIGHTEDGRAPH_H

#include <stdexcept>
#include <iostream>
#include <climits>    // For INT_MAX
#include <string>     // Used for labels

using namespace std;

struct info {
	
	int weight;
	int localPreference;
	int MED;
	string origin;
	string community;
};

class WeightedGraph
{
	public:
	
		static const int MAX_GRAPH_SIZE = 10,
		INFINITE_EDGE_WT = INT_MAX;  

		class Vertex
		{
			public:
			
				Vertex() {
					
					numberOfHosts = 0;
					bgpSession = false;
				}
				void setLabel( const string& newLabel ) { 
					
					label = newLabel;
				}
				string getLabel() const { 
					
					return label; 
				}

				string subnet[ 10 ];
				string label;
				int numberOfHosts;
				bool bgpSession;
				info attributes;
		};

    //--------------------------------------------------------------------

		// Constructors
		WeightedGraph ( int maxNumber = MAX_GRAPH_SIZE );
		WeightedGraph ( const WeightedGraph& other );
		// Overloaded assignment operator
		WeightedGraph& operator= ( const WeightedGraph& other );

		// Destructor
		~WeightedGraph ();

		// Graph manipulation operations
		void insertVertex ( const Vertex& newVertex )            // Insert vertex
			throw ( logic_error );
		void insertEdge ( const string& v1, const string& v2, int wt )    // Insert edge
			throw ( logic_error );
		bool retrieveVertex ( const string& v, Vertex& vData ) const; // Get vertex
		bool getEdgeWeight ( const string& v1, const string& v2, int& wt ) const
			throw ( logic_error );                        // Get edge wt.
		void removeVertex ( const string& v )                     // Remove vertex
			throw ( logic_error );
		void removeEdge ( const string& v1, const string& v2 )            // Remove edge
			throw ( logic_error );
		void clear ();                                    // Clear graph

		// Graph status operations
		bool isEmpty () const;                            // Graph is empty
		bool isFull () const;                             // Graph is full

		// Output the graph structure -- used in testing/debugging
		void showStructure() const;

		// Programming exercise operations
		void showShortestPaths ();
		
		int ASnumber;
		string ipPrefix;
		int bgpIndex;

		// Facilitator functions
		int getIndex ( const string& v ) const;          // Converts vertex label to an
												   //   adjacency matrix index                                   
		int getEdge ( int row, int col ) const;          // Get edge weight using
												   //   adjacency matrix indices
		void setEdge ( int row, int col, int wt);  // Set edge weight using
												   //   adjacency matrix indices
		// Data members
		int maxSize,          // Maximum number of vertices in the graph
			size;             // Actual number of vertices in the graph
		Vertex *vertexList;   // Vertex list
		int *adjMatrix;       // Adjacency matrix
		int *pathMatrix;      // Path matrix
};

#endif		// #ifndef WEIGHTEDGRAPH_H
