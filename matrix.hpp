#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace atter{
	template<typename T>
	class matrix{
		public:
		matrix() : _rows{0}, _cols{0}, _data{nullptr} {}
		
		~matrix(){
			delete[] _data;
		}
		
		matrix(const size_t rows, const size_t cols)
			: _rows{rows}, _cols{cols} {
			_data = new T[_rows * _cols];
		}
		
		matrix(const std::initializer_list<std::initializer_list<T>>& list)
			//_cols: Assuming (technically wrongly) that all lists in list have the same length.
			: _rows{list.size()}, _cols{list.begin()->size()} {
			_data = new T[_rows * _cols];
			
			size_t row = 0;
			for(auto row_list : list){
				size_t col = 0;
				for(auto col_value : row_list){
					(*this)(row, col++) = col_value;
				}
				++row;
			}
		}
		
		matrix(const matrix& other)
			: _rows{other._rows}, _cols{other._cols} {
			_data = new T[_rows * _cols];
			for(size_t n = 0; n < _rows * _cols; ++n){
				this->_data[n] = other._data[n];
			}
		}
		
		matrix(matrix&& other) noexcept
			: matrix{} {
			swap(*this, other);
		}
		
		matrix& operator=(matrix other){
			//Copy-and-swap style. Note that other is taken by value.
			swap(*this, other);
			return *this;
		}
		
		//Easy way to initialize all elements to a single value.
		matrix& operator=(const T&& rhs){
			for(size_t n = 0; n < _rows * _cols; ++n){
				_data[n] = rhs;
			}
			return *this;
		}
		
		matrix& operator+=(const matrix& rhs){
			if(this->_rows != rhs._rows || this->_cols != rhs._cols){
				throw std::invalid_argument{"Error: Dimension mismatch."};
			}
			for(size_t n = 0; n < _rows * _cols; ++n){
				_data[n] += rhs._data[n];
			}
			return *this;
		}
		
		matrix operator+(const matrix& rhs){
			matrix sum{*this};
			sum += rhs;
			return sum;
		}
		
		matrix& operator*=(const matrix& rhs){
			(*this) = (*this) * rhs;
			return (*this);
		}
		
		matrix operator*(const matrix& rhs){
			//Matrix multiplication: (M x K) * (K x N) => (M x N)
			if(this->_cols != rhs._rows){
				throw std::invalid_argument{"Error: Dimension mismatch."};
			}
			matrix product{this->_rows, rhs._cols};
			for(size_t lhs_row = 0; lhs_row < this->_rows; ++lhs_row){
				for(size_t rhs_col = 0; rhs_col < rhs._cols; ++rhs_col){
					T current_element = 0;
					for(size_t k = 0; k < this->_cols; ++k){ //this->_cols == rhs._rows
						current_element += ((*this)(lhs_row, k) * rhs(k, rhs_col));
					}
					product(lhs_row, rhs_col) = current_element;
				}		
			}
			return product;
		}
		
		//Using () as a makeshift 2-argument subscript operator.
		T& operator()(size_t row, size_t col){
			return _data[row * _cols + col];
		}
		
		const T operator()(size_t row, size_t col) const {
			return _data[row * _cols + col];
		}
		
		bool operator!=(const matrix& other) const {
			return !((*this) == other);
		}
		
		bool operator==(const matrix& other) const {
			if(this == &other){ //Is the matrix being compared to itself?
				return true;
			}
			else if(_rows != other._rows || _cols != other.cols){
				return false; //Return early if dimensions aren't equal.
			}
			
			for(size_t n = 0; n < _rows * _cols; ++n){
				if(_data[n] != other._data[n]){
					return false; //Return early if an unequal element is found.
				}
			}
			return true; //All elements match.
		}
		
		friend void swap(matrix& lhs, matrix& rhs) noexcept {
			using std::swap;
			swap(lhs._rows, rhs._rows);
			swap(lhs._cols, rhs._cols);
			swap(lhs._data, rhs._data);
		}
		
		std::string to_string() const {
			std::ostringstream oss;
			for(size_t row = 0; row < _rows; ++row){
				for(size_t col = 0; col < _cols; ++col){
					oss << (*this)(row, col) << ' ';
				}
				oss << '\n';
			}
			oss << "Rows: " << _rows << ", cols: " << _cols << '\n';
			return oss.str();
		}
		
		private:
		size_t _rows, _cols;
		T* _data;
	};
}
#endif