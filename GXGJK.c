#include <G10/GXGJK.h>

vec3 find_extreme_point ( GXCollider_t *collider, vec3 direction ) 
{ 
	vec3 ret = { 0.f, 0.f, 0.f };
	
	float max_d = -FLT_MAX;

	for (size_t i = 0; i < collider->convex_hull_count; i++)
	{
		float d = dot_product_vec3(direction, collider->convex_hull[i]);
		if (d > max_d)
		{
			max_d = d,
			ret   = collider->convex_hull[i];
		}
	}

	return ret;
}

vec3 support(GXCollision_t *collision, vec3 direction)
{
	vec3 ret = { 0.f,0.f,0.f };

	sub_vec3(&ret, find_extreme_point(collision->a->collider, direction), find_extreme_point(collision->b->collider, mul_vec3_f(direction, -1)));

	return ret;
}

bool GJK(GXCollision_t* collision)
{

	// Commentary
	{
		/*
		 * This is an implementation of the Gilbert Johnson Keerthi algorithm for convex 
		 * hull collision detection. 
		   
		    E. G. Gilbert, D. W. Johnson and S. S. Keerthi, "A fast procedure for computing the distance between 
		    complex objects in three-dimensional space," in IEEE Journal on Robotics and Automation, vol. 4, no.
		    2, pp. 193-203, April 1988, doi: 10.1109/56.2083.
		   
		 */
	}

	// Argument check
	{
		if (collision == (void*)0)
			return 0;
		if (collision->a == (void*)0)
			return 0;
		if (collision->b == (void*)0)
			return 0;
		if (collision->a->collider->convex_hull == (void*)0)
			return 0;
		if (collision->b->collider->convex_hull == (void*)0)
			return 0;
	}

	// Initialized data
	GXCollider_t *a = collision->a->collider,
		         *b = collision->b->collider;

	vec3 s  = support(collision, (vec3) { 1.f, 0.f, 0.f });
	bool ret = false;
	vec3 *p = calloc(4, sizeof(vec3));
	
	p[0] = s;

	vec3 direction = mul_vec3_f(s, -1);

	while (true)
	{
		s = support(collision, direction);

		if (dot_product_vec3(s, direction) <= 0)
			return false;

		p[1] = s;

		if (next_simplex(p, direction))
			return true;

	}

}

bool next_simplex ( vec3 *points, vec3 direction )
{
	size_t size = 0;
	while (&points[size++]);

	switch (size)
	{
		case 2:
			return simplex_line(points, direction);
		case 3:
			return simplex_triangle(points, direction);
		case 4:
			return simplex_tetrahedron(points, direction);

		default:
			g_print_error("[G10] [GJK] Default on next simplex\n");
	}

	// ERROR
	return false;
}

bool same_direction(vec3 a, vec3 b)
{
	return (dot_product_vec3(a, b) > 0) ? true : false;
}

bool simplex_line(vec3* points, vec3 direction)
{
	vec3 a  = points[0],
		 b  = points[1],
		 ab,
		 na = mul_vec3_f(a, -1);

	sub_vec3(&ab, b, a);

	if (same_direction(ab, na))
		direction = cross_product_vec3(cross_product_vec3(ab, na), ab);
	else
	{
		points[0] = a,
		direction = na;
	}

	return false;
}

bool simplex_triangle(vec3* points, vec3 direction)
{
	vec3 a   = points[0], 
		 b   = points[1],
		 c   = points[2];

	vec3 ab,
		 ac;

	sub_vec3(&ab, b, a);
	sub_vec3(&ac, c, a);

	vec3 na  = mul_vec3_f(a,-1),
		 abc = cross_product_vec3(ab,ac);

	if (same_direction(cross_product_vec3(abc, ac), na))
	{
		if (same_direction(ac, na))
		{
			points[0] = a,
			points[1] = c;
			
			direction = cross_product_vec3(cross_product_vec3(ac,na),ac);
		}
		else
		{
			points[0]=a,
			points[1]=b;
			return simplex_line(points, direction);
		}
	}
	else
	{
		if (same_direction(cross_product_vec3(ab, abc), na))
		{
			points[0]=a,
			points[1]=b;
			return simplex_line(points, direction);
		}
		else
		{
			if (same_direction(abc, na))
			{
				direction = abc;
			}
			else
			{
				points[0]=a,
				points[1]=c,
				points[2]=b;

				direction = mul_vec3_f(abc, -1);
			}
		}
	}

	return false;
}

bool simplex_tetrahedron(vec3* points, vec3 direction)
{
	vec3 a  = points[0],
		 b  = points[1],
		 c  = points[2],
		 d  = points[3],
		 na = mul_vec3_f(a, -1);

	vec3 ab,
		 ac,
		 ad;
	
	sub_vec3(&ab, b, a);
	sub_vec3(&ac, d, a);
	sub_vec3(&ad, c, a);


	vec3 abc = cross_product_vec3(ad, ab),
		 acd = cross_product_vec3(ad, ab),
		 adb = cross_product_vec3(ad, ab);

	if (same_direction(abc, na))
	{
		points[0] = a,
		points[1] = b,
		points[2] = c;

		return simplex_triangle(points, direction);
	}

	if (same_direction(acd, na))
	{
		points[0] = a,
		points[1] = c,
		points[2] = d;

		return simplex_triangle(points, direction);
	}

	if (same_direction(adb, na))
	{
		points[0] = a,
		points[1] = d,
		points[2] = b;

		return simplex_triangle(points, direction);
	}

	return true;
}