
SELECT t_user.age, AVG(rate) AS avgrate
	FROM hive_sql_test1.t_rating AS t_rating
	LEFT JOIN hive_sql_test1.t_user AS t_user
	ON t_user.userid=t_rating.userid
	WHERE t_rating.movieID='2116'
	GROUP BY t_user.age;


SELECT t_m_user.sex, t_movie.moviename,
		AVG(t_rating.rate) AS avgrate,
		COUNT(*) AS total
	FROM
		(SELECT sex, userid
			FROM hive_sql_test1.t_user AS t_user
			WHERE t_user.sex='M') AS t_m_user
		INNER JOIN hive_sql_test1.t_rating AS t_rating
			ON t_m_user.userid=t_rating.userid
		INNER JOIN hive_sql_test1.t_movie AS t_movie
			ON t_movie.movieid=t_rating.movieid
	GROUP BY t_movie.movieid
	HAVING total>50
	ORDER BY avgrate DESC;
	


