#ifndef MATHSPECIFIC_H
#define MATHSPECIFIC_H

#include <math.h>
#include <limits>

using std::vector;
using std::next_permutation;
using std::sort;
using std::fill;
using std::runtime_error;

namespace KMath {
	namespace Specific {
		
		/**
		 * Exception class for cases that k combinations of n are required and k is greater than n!
		 */
		class k_GreaterThan_n_Exception : public runtime_error {
			public:
				k_GreaterThan_n_Exception() : runtime_error("k_GreaterThan_n_Exception: k > n! (must be k <= n)") { }
		};
		
		/**
		 * @fn vector< vector<T> > permutations(vector<T> &v)
		 * @brief Returns all possible permutations of elements given a vector.
		 */
		template <typename T>
		vector< vector<T> > permutations(vector<T> &v) {

			unsigned int next = 0;
			vector< vector<T> > perms( factorial(v.size()) );
	
			sort(v.begin(), v.end());
	
			do {
				perms[next] = v;
				next++;
			} while(next_permutation(v.begin(), v.end()));
	
			return perms;
		}
		
		/**
		 * @fn vector< vector<T> > combinations(vector<T> &v, unsigned int k)
		 * @brief Returns all possible k-combinations of distinct k-element subsets 
		 * that can be formed, given a vector. 
		 */
		template <typename T>
		vector< vector<T> > combinations(vector<T> &v, unsigned int k) {
	
			unsigned int next = 0;
			
			if(k > v.size())
				throw k_GreaterThan_n_Exception();
				
			vector< vector<T> > combs( binomialCoefficient(v.size(), k) );
			
			vector<bool> binary(v.size());
			fill(binary.end() - k, binary.end(), true);
	
			do {
			   for(unsigned int i = 0 ; i < v.size() ; i++) {
				   if(binary[i])
					   combs[next].insert(combs[next].end(), v[i]);
			   }
			   next++;
			} while(next_permutation(binary.begin(), binary.end()));
			
			return combs;
		}
		
		/**
		 * @fn vector< vector<T> > permutationsOfCombinations(vector<T> &v, unsigned int k)
		 * @brief Combination of the 2 above functions. Returns all possible permutations for
		 * all the k-combinations of distinct subsets of the given vector.
		 */
		template <typename T>
		vector< vector<T> > permutationsOfCombinations(vector<T> &v, unsigned int k) {
	
			unsigned int next = 0;
			vector< vector<T> > result, combs;
			
			if(k > v.size())
				throw k_GreaterThan_n_Exception();
			else if(k == v.size())
				return permutations(v);
			else
				combs = combinations(v, k);
		
			for(unsigned int it = 0 ; it < combs.size() ; it++) {
				sort(combs[it].begin(), combs[it].end());
				do {
					result.resize(result.size() + 1);
					result[next] = combs[it];
					next++;
				} while(next_permutation(combs[it].begin(), combs[it].end()));
			}
	
			return result;
		}
	
	};
};

#endif
