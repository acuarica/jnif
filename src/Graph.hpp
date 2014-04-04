/*
 * Graph.hpp
 *
 *  Created on: Apr 3, 2014
 *      Author: luigi
 */

#ifndef JNIF_GRAPH_HPP
#define JNIF_GRAPH_HPP

#include <ostream>

namespace jnif {

template<typename TNode>
class Graph {
public:

	class NodeKey {
		friend class Graph;

	public:
		NodeKey next() const {
			return NodeKey(_index + 1);
		}

		NodeKey(int index) :
				_index(index) {
		}

		int _index;
	private:
	};

	class NodeIterator {
		friend Graph;
	public:
		bool operator!=(const NodeIterator& other) const {
			return current._index != other.current._index;
		}

		NodeKey operator*() {
			return current;
		}

		void operator++() {
			current._index++;
		}

	private:
		NodeIterator(NodeKey current) :
				current(current) {
		}

		NodeKey current;
	};

	class EdgeIterator {
		friend Graph;
	public:
		bool operator!=(const EdgeIterator& other) const {
			return it != other.it;
		}

		NodeKey operator*() {
			return *it;
		}

		void operator++() {
			it++;
		}

	private:
		EdgeIterator(std::set<int>::iterator it) :
				it(it) {
		}
		std::set<int>::iterator it;
	};

	class EdgeIterable {
		friend Graph;
	public:
		EdgeIterator begin() const {
			return EdgeIterator(value.begin());
		}

		EdgeIterator end() const {
			return EdgeIterator(value.end());
		}

	private:
		EdgeIterable(std::set<int> value) :
				value(value) {

		}
		std::set<int> value;
	};

	NodeKey addNode(const TNode& nodeValue) {
		NodeKey nodeKey(nodes.size());
		nodes.emplace_back(nodeValue);
		return nodeKey;
	}

	inline int nodeCount() const {
		return nodes.size();
	}

	TNode& getNode(NodeKey nodeId) {
		return nodes[nodeId._index].value;
	}

	void addEdge(NodeKey to, NodeKey from) {
		nodes[to._index].outEdges.addEdge(from._index);
		nodes[from._index].inEdges.addEdge(to._index);
	}

	inline NodeIterator begin() {
		return NodeIterator(0);
	}

	inline NodeIterator end() {
		return NodeIterator(nodeCount());
	}

	EdgeIterable outEdges(NodeKey nodeKey) const {
		return EdgeIterable(nodes[nodeKey._index].outEdges.edges);
	}

	EdgeIterable inEdges(NodeKey nodeId) const {
		return EdgeIterable(nodes[nodeId._index].inEdges.edges);
	}

private:

	struct EdgeSet {
		void addEdge(NodeKey nodeId) {
			edges.insert(nodeId._index);
		}

		std::set<int> edges;
	};

	struct Node {
		Node(const TNode& value) :
				value(value) {
		}
		TNode value;
		EdgeSet outEdges;
		EdgeSet inEdges;
	};

	std::vector<Node> nodes;
};

}

#endif
