#pragma once
template <typename T>
class QuadTree
{
public:
	inline QuadTree(Rectangle area) : m_area(area), m_hasChildren(false) {}
	inline void Insert(Vector2 & pos, T & leaf) {
		if (m_hasChildren) {
			long half = m_area.width / 2;
			if (pos.x > m_area.x + half) {
				if (pos.y > m_area.y + half) {
					m_I->Insert(pos, leaf);
				}
				else {
					m_IV->Insert(pos, leaf);
				}
			}
			else {
				if (pos.y > m_area.y + half) {
					m_II->Insert(pos, leaf);
				}
				else {
					m_III->Insert(pos, leaf);
				}
			}
		}
		else {
			m_leaves.push_back(std::make_pair(pos,leaf));
		}
	} 
	inline void Remove(Rectangle & searchArea) {
		if (m_area.Intersects(searchArea)) {
			if (m_hasChildren) {
				// Recursively search children
				m_I->Remove(searchArea);
				m_II->Remove(searchArea);
				m_III->Remove(searchArea);
				m_IV->Remove(searchArea);
			}
			else {
				// Remove all leaves
				m_leaves.clear();
			}
		}
	}
	inline void Find(Rectangle & searchArea, vector<T> & results) {
		if (m_area.Intersects(searchArea)) {
			if (m_hasChildren) {
				// Recursively search children
				m_I->Find(searchArea, results);
				m_II->Find(searchArea, results);
				m_III->Find(searchArea, results);
				m_IV->Find(searchArea, results);
			}
			else {
				// Add all leaves to the result set
				for (auto & leaf : m_leaves) {
					results.push_back(leaf.second);
				}
			}
		}
	}
	// Adds four leaves and re-inserts children into those leaves
	// Returns the width of the child quads
	inline void Grow(int & minQuadWidth)
	{
		if (m_area.width > minQuadWidth) {
			long half = m_area.width / 2;
			// Create children
			m_I = make_unique<QuadTree>(Rectangle(m_area.x + half, m_area.y + half, half, half));
			m_II = make_unique<QuadTree>(Rectangle(m_area.x, m_area.y + half, half, half));
			m_III = make_unique<QuadTree>(Rectangle(m_area.x, m_area.y, half, half));
			m_IV = make_unique<QuadTree>(Rectangle(m_area.x + half, m_area.y, half, half));
			m_hasChildren = true;
			// Grow children recursively
			m_I->Grow(minQuadWidth);
			m_II->Grow(minQuadWidth);
			m_III->Grow(minQuadWidth);
			m_IV->Grow(minQuadWidth);
			// Insert all leaves into children
			for (auto & leaf : m_leaves) {
				Insert(leaf.first, leaf.second);
			}
			// Clear the leaves for this level
			m_leaves.clear();
		}
	}
	// Removes children and retrieves their leaves
	inline void Recede() {
		// retrieve all child leaves
		RetrieveLeaves(m_I);
		RetrieveLeaves(m_II);
		RetrieveLeaves(m_III);
		RetrieveLeaves(m_IV);
		// delete the children
		m_I.reset();
		m_II.reset();
		m_III.reset();
		m_IV.reset();
		m_hasChildren = false;
	}
	vector<std::pair<Vector2, T>> & GetLeaves() {
		return m_leaves;
	}
private:
	bool m_hasChildren;
	std::unique_ptr<QuadTree> m_I;
	unique_ptr<QuadTree> m_II;
	unique_ptr<QuadTree> m_III;
	unique_ptr<QuadTree> m_IV;
	vector<std::pair<Vector2,T>> m_leaves;
	Rectangle m_area;

	void RetrieveLeaves(std::unique_ptr<QuadTree> & child) {
		for (auto & leaf : child->GetLeaves()) {
			m_leaves.push_back(leaf);
		}
	}
};

