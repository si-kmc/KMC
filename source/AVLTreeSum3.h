#pragma once


#include <algorithm>


/*
AVL木(AVLTree.h)に、KMC用のイベント発生率の和自動でとる機能を加えたもの.
rankの更新と同じタイミングで自身のイベント発生率と子のイベント発生率の和(total_ratio)を更新すればよいはず.
Rがイベント発生率に相当する型

*/


template <typename K, typename R, typename T>
class AVLTreeSum3 {
private:
	struct AVLNode;
public:
	AVLTreeSum3() : head(nullptr) {};
	~AVLTreeSum3() {};

	struct Iterator;

	/*
	STLとは異なりすでに登録されているキーならupdateする
	*/
	Iterator Insert(const K& key, const R& ratio, const T& element) {

		AVLNode* t = mInsert(key, ratio, element);
		return Iterator(t);

	};

	Iterator Find(const K& key) {
		AVLNode* t = mFind(key);
		return Iterator(t);
	};

	bool Erase(Iterator& itr) {
		return mErase(itr);
	};

	bool Erase(const K& key) {
		return mErase(mFind(key));
	}

	void Update(Iterator& itr, const R& ratio, const T& element) {
		mUpdate(itr, ratio, element);
	};

	void Update(Iterator& itr, const R& ratio) {
		mUpdate(itr, ratio);
	};


	T* Max() {
		AVLNode* t = mMax(head);
		if (t) {
			return &(t->element);
		}
		return NULL;
	};

	T* Min() {
		K key;
		return Min(&key);
	};

	T* Min(K* key) {
		AVLNode* t = mMin(head);
		if (t) {
			*key = t->key;
			return &(t->element);
		}
		return NULL;
	};

	/*
	テスト用関数
	*/
	bool Check() {
		return mCheck(head);
	}

	/* 以下はAVLTreeにはない拡張機能 *****************************/

	/*
	全イベントの全確率を返す
	*/
	R TotalRatio() {
		if (head) {
			return head->TotalRatio();
		}
		return 0.0;
	}

	/*
	その要素までのイベント発生確率の和が指定した数を超える最小のkeyの要素を返す
	*/
	Iterator Bring(const R& bring_ratio, R* residual) {

		AVLNode* node = mBring(bring_ratio, residual);
		return Iterator(node);
	}

	Iterator Bring(const R& bring_ratio, K* key) {
		R residual;
		return Bring(bring_ratio, key, &residual);
	}
#if 0
	Iterator Begin() const {		
		return Iterator(head);
	}

	Iterator End() const {		
		return Iterator(nullptr);
	}
#else
	Iterator Begin() const {
		if(head==nullptr){
			return Iterator(nullptr); 
		}
		AVLNode* node = head;
		while(node->left){
			node = node->left;
		}
		return Iterator(node);
	}

	Iterator End() const {		
		return Iterator(nullptr);
	}
#endif

private:



	struct AVLNode {
		K key;
		T element;
		R ratio;
		int rank;
		R left_ratio;
		R right_ratio;
		AVLNode* up;
		AVLNode* left;
		AVLNode* right;
		//int diff_rank_RL;  //left->rank - right->rank

		/*
		AVLNode(const K& key_, const T& element_, AVLNode* up_, AVLNode* left_, AVLNode* right_ ) :
		key(key_), element(element_), up(up_), left(left_), right(right_), rank(0)
		{		};
		*/

		AVLNode(const K& key_, const R& ratio_, const T& element_, AVLNode* up_) :
			key(key_), element(element_), up(up_), left(NULL), right(NULL), rank(1),
			ratio(ratio_), left_ratio(0.0), right_ratio(0.0)
		{		};

		void LinkLeft(AVLNode* target) {
			this->left = target;
			if (target) target->up = this;
		}

		void LinkRight(AVLNode* target) {
			this->right = target;
			if (target) target->up = this;
		}


		void ResetRatio() {
			//total_ratio = ratio;
			if (left) {
				left_ratio = left->TotalRatio();
			} else {
				left_ratio = 0.0;
			}
			if (right) {
				right_ratio = right->TotalRatio();
			} else {
				right_ratio = 0.0;
			}
		}

		double TotalRatio() {
			return ratio + left_ratio + right_ratio;
		}

	};

	AVLNode* head;

	int mRank(const AVLNode* t) {
		if (t == NULL) return 0;
		return t->rank;
	}



	AVLNode* mFind(const K& key) {
		AVLNode* parent;
		return mFind(key, parent);

	}

	AVLNode* mFind(const K& key, AVLNode*& parent) {

		parent = NULL;

		if (head == NULL) {
			return NULL;
		}

		AVLNode* current = head;
		while (current) {
			if (key == current->key) {
				return current;
			} else if (key < current->key) {
				parent = current;
				current = current->left;
			} else { //case that key > current->key//
				parent = current;
				current = current->right;
			}
		}

		return NULL;
	};

	AVLNode* mUpdate(AVLNode* node, const R& ratio, const T& element) {
		node->element = element;
		node->ratio = ratio;
		mResetOnlyRatioFromFoot(node);//自分を含めratioをbottom up更新//
		return node;
	}

	AVLNode* mUpdate(AVLNode* node, const R& ratio) {
		
		node->ratio = ratio;
		mResetOnlyRatioFromFoot(node);//自分を含めratioをbottom up更新//
		return node;
	}

	AVLNode* mInsert(const K& key, const R& ratio, const T& element) {

		if (head == NULL) {
			head = new AVLNode(key, ratio, element, NULL);
			return head;
		}

		AVLNode* parent;
		AVLNode* current = mFind(key, parent);
		if (current) {
			return mUpdate(current, ratio, element);
			/*
			current->element = element;
			current->ratio = ratio;
			mResetOnlyRatioFromFoot(current);//自分を含めratioをbottom up更新//
			return current;
			*/
		} else {

			if (key < parent->key) {
				AVLNode* node = new AVLNode(key, ratio, element, parent);
				parent->left = node;
				mResetRankAfterInsert(parent);
				return node;

			} else { //case that key > parent->key//
				AVLNode* node = new AVLNode(key, ratio, element, parent);
				parent->right = node;
				mResetRankAfterInsert(parent);
				return node;
			}
		}

		return NULL;
	};



	/*
	bottomノードから上へ辿りながらランクを再設定
	ランクは左右の子のランクで比較した大きい方+1
	*/
	void mResetRankAfterInsert(AVLNode* bottom)
	{
		mResetRankFromFoot(bottom, false);
	}

	void mResetRankAfterErase(AVLNode* bottom)
	{
		mResetRankFromFoot(bottom, true);
	}

	void mResetRankFromFoot(AVLNode* bottom, bool continuous_flag)
	{
		bool flag_change_rank = true;
		AVLNode* node = bottom;
		while (node) {

			node->ResetRatio();

			if (flag_change_rank) {
				int old_rank = node->rank;


				node->rank = 1 + std::max(mRank(node->left), mRank(node->right));





				AVLNode* up = node->up;
				AVLNode** p_child = (up == NULL ? &head : (up->left == node ? &(up->left) : &(up->right)));
				//回転によりバランスを取った新しい頂点//
				node = mBalance(node);
				node->up = up;
				(*p_child) = node;


				if (old_rank == node->rank) {
					flag_change_rank = continuous_flag;
				}
			}
			node = node->up;
		}
	};

	/*
	ratioのみのbottom-up計算をやり直す
	*/
	void mResetOnlyRatioFromFoot(AVLNode* bottom)
	{
		bool flag_change_rank = true;
		AVLNode* node = bottom;
		while (node) {

			node->ResetRatio();
			node = node->up;
		}
	};

	/*
	左右のノードのランクに差があったら回転してバランスを取る
	新しい親(top)を返す
	*/
	AVLNode* mBalance(AVLNode* link) {
		if (link == NULL) return NULL;

		AVLNode* u = link;
		int u_diff = mRank(u->left) - mRank(u->right);
		if (u_diff == 2) {
			AVLNode* v = u->left; //always is not null//
			int v_diff = mRank(v->left) - mRank(v->right);
			if (v_diff == -1) {
				AVLNode* w = v->right;
				v->LinkRight(w->left);
				//v->right = w->left;
				//v->right->up
				u->LinkLeft(w->right);
				//u->left = w->right;
				w->LinkLeft(v);
				w->LinkRight(u);
				//v->up = w;
				//u->up = w;

				u->rank = 1 + std::max(mRank(u->left), mRank(u->right));
				u->ResetRatio();
				v->rank = 1 + std::max(mRank(v->left), mRank(v->right));
				v->ResetRatio();
				w->rank = 1 + std::max(v->rank, u->rank);
				w->ResetRatio();



				return w;

			} else
#ifdef _DEBUG
				if ((v_diff == 1) || (v_diff == 0))
#endif
				{

					u->LinkLeft(v->right);
					//u->left->up = u;
					//v->right = u;
					v->LinkRight(u);
					//v->right->up = v;

					u->rank = 1 + std::max(mRank(u->left), mRank(u->right));
					u->ResetRatio();
					v->rank = 1 + std::max(mRank(v->left), u->rank);
					v->ResetRatio();

					return v;

				}
#ifdef _DEBUG
				else {
					//AVL木ではここで (v_diff == +-2) は起こり得ない。//

					printf("error: barance u_diff=2, v_diff=%d\n", v_diff);//error/
				}
#endif
		} else if (u_diff == -2) {
			AVLNode* v = u->right; //always is not null//
			int v_diff = mRank(v->left) - mRank(v->right);
			if (v_diff == 1) {
				AVLNode* w = v->left;
				v->LinkLeft(w->right);
				//v->left = w->right;
				u->LinkRight(w->left);
				//u->right = w->left;
				w->LinkRight(v);
				//w->right = v;
				w->LinkLeft(u);
				//w->left = u;
				//v->up = w;
				//u->up = w;


				u->rank = 1 + std::max(mRank(u->left), mRank(u->right));
				u->ResetRatio();
				v->rank = 1 + std::max(mRank(v->left), mRank(v->right));
				v->ResetRatio();
				w->rank = 1 + std::max(v->rank, u->rank);
				w->ResetRatio();

				return w;

			} else
#ifdef _DEBUG
				if ((v_diff == -1) || (v_diff == 0))
#endif
				{

					u->LinkRight(v->left);
					//u->right = v->left;
					//u->right->up = u;
					v->LinkLeft(u);
					//v->left = u;
					//v->left->up = v;

					u->rank = 1 + std::max(mRank(u->left), mRank(u->right));
					u->ResetRatio();
					v->rank = 1 + std::max(u->rank, mRank(v->right));
					v->ResetRatio();

					return v;
				}
#ifdef _DEBUG
				else {
					//AVL木ではここで (v_diff == 0) は起こり得ない。//

					printf("error: barance u_diff=2, v_diff=%d\n", v_diff);//error/
				}
#endif


		}

		return u;
	};



	AVLNode* mMax(AVLNode* target) {
		AVLNode* node = target;
		while (node->right) {

			node = node->right;
		}
		return node;
	}



	AVLNode* mMin(AVLNode* target) {
		AVLNode* node = target;
		while (node->left) {

			node = node->left;
		}
		return node;
	}

	bool mCheck(AVLNode* node) {
		if (node == NULL) return true;


		if (node->left) {
			if (mCheck(node->left) == false) return false;
			if (node->left->key > node->key) return false;
		}
		if (node->right) {
			if (mCheck(node->right) == false) return false;
			if (node->right->key < node->key) return false;
		}

		int l_rank = mRank(node->left);
		int r_rank = mRank(node->right);
		int diff = l_rank - r_rank;
		if ((diff < -1) || (diff > 1)) return false;
		if (node->rank != 1 + std::max(l_rank, r_rank)) return false;

		return true;


	};

	AVLNode* mBring(const R& bring_ratio, R* residual) {
		if (head == NULL) {
			return NULL;
		}

		R sum = 0.0;

		AVLNode* node = head;
		while (1) {

			if (bring_ratio <= sum + node->left_ratio) {
				node = node->left;
				continue;
			} else {
				sum += node->left_ratio;
			}

			if (bring_ratio <= sum + node->ratio) {
				*residual = bring_ratio - sum;
				return node;
			}

			sum += node->ratio;

			if (node->right == NULL) {
				*residual = bring_ratio - sum;
				return node;
			}

			node = node->right;
		}

	};


	bool mErase(AVLNode* t) {
		if (t == NULL) {
			return false;
		}
		AVLNode* parent = t->up;


		AVLNode* start_reset = parent;
		AVLNode* substitute;
		if (t->left == NULL) {


			substitute = t->right;



		} else {
			if (t->left->right == NULL) {
				substitute = t->left;
				substitute->LinkRight(t->right);
				start_reset = substitute;
			} else {
				substitute = mMax(t->left->right);
				substitute->up->LinkRight(substitute->left);
				//substitute->up->right = substitute->left;

				start_reset = substitute->up;

				substitute->LinkLeft(t->left);
				//substitute->left = t->left;
				substitute->LinkRight(t->right);
				//substitute->right = t->right;

				//substitute->rank = t->rank;
			}
		}


		if (parent == NULL) {
			head = substitute;
			if (substitute) substitute->up = NULL;
		} else if (parent->left == t) {
			parent->LinkLeft(substitute);
		} else {
			parent->LinkRight(substitute);
		}


		delete t;

		if (start_reset) {
			mResetRankAfterErase(start_reset);
		}


		return true;
	};

public:


	struct Iterator {
	private:
		AVLNode* node;
	public:
		Iterator() : node(NULL)
		{	};

		Iterator(AVLNode* node_) : node(node_)
		{	};

		bool IsNull() const {
			return (node == NULL);
		}

		const K& key() const {
			return node->key;
		}

		T& value() const {
			return node->element;
		}

		const R& ratio() const{
			return node->ratio;
		}

		//cast operator//
		operator AVLNode*() const {
			return node;
		}

#if 0
		//incremental//
		Iterator operator++(){
			if (this->node->left) {
				this->node = this->node->left;
				return *this;
			}else if (this->node->right) {
				this->node = this->node->right;
				return *this;				
			}else{
				this->node = this->node->up;
				while (this->node) {
					if (this->node->right) {
						this->node = this->node->right;
						return *this;	
					}
					this->node = this->node->up;
				}
				//here, node == nullptr//
				return *this;				
			}
		}

		bool operator==(const Iterator& target) const {
			return this->node == target.node;
		}
		
		bool operator!=(const Iterator& target) const {
			return !(this->node == target.node);
		}

#else
		//incremental//
		Iterator operator++(){
			if(this->node->right){
				this->node = this->node->right;
				while(this->node->left){
					this->node = this->node->left;
				}
				return *this;
			}else{
				AVLNode* prev = nullptr;				
				while(this->node->right == prev){
					prev = this->node;
					this->node = this->node->up;
					if(this->node == nullptr){
						return *this;
					}
				}
				return *this;
			}
		}

		bool operator==(const Iterator& target) const {
			return this->node == target.node;
		}
		
		bool operator!=(const Iterator& target) const {
			return !(this->node == target.node);
		}
#endif
	};

};

