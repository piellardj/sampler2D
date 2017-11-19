/**
    @author Jérémie Piellard
    @date 19 november 2017 
*/

#ifndef SAMPLER_2D_HPP
#define SAMPLER_2D_HPP


#include <vector>


template<typename T>
struct vec2
{
	vec2(T a, T b) : x(a), y(b) {}
	T x, y;
};

typedef vec2<unsigned int> uvec2;
typedef vec2<float> fvec2;

/*! \class Sampler2D
 * \brief Class for sampling discrete 2D density distributions. */
class Sampler2D
{
public:

	/*!
	 * If parameters are invalid, the sampler will sample the uniform distribution.
	 * \param density : Doesn't have to be normalized. Negative values are set to 0.
	 */
	Sampler2D(uvec2 const& size, std::vector<float> density);

	/*!
	 * \brief Generates a sample using inverse method.
	 * \return a sample in [0,1]x[0,1]
	 */
	fvec2 sample() const;

private:
	uvec2 _size;
	std::vector<std::vector<float>> _normalizedLines;
	std::vector<float> _cumulatedLines;  //1D grid, _Yrow[0] = _density[0][0] + _density[1][0] + ...
};

#endif // SAMPLER_2D_HPP