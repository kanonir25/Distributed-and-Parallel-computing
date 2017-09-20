add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.9.0.jar;
USE osipinius;

DROP TABLE if exists prequer; 
CREATE TABLE prequer as
SELECT region, count(1) as amount
FROM user_logs logs JOIN ip_data dat ON (logs.ip = dat.ip)
GROUP BY region;

SELECT region, amount, avgAmount
FROM	(
	SELECT region, amount, avg(amount) over() as avgAmount
	FROM prequer
	) AS avgRegions
WHERE avgRegions.amount > avgRegions.avgAmount;
