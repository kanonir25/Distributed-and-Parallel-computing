add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.9.0.jar;
USE osipinius;

SELECT *
FROM 	(
	SELECT region, sex, count(1)
	FROM 	(
		SELECT dat.ip, dat.region, us.sex
		FROM  ip_data dat JOIN user_data us on (dat.ip = us.ip)
		) rex JOIN user_logs logs ON (logs.ip = rex.ip)
	GROUP BY region, sex
	) as new;
