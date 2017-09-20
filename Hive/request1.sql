add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.9.0.jar;
USE osipinius;

SELECT time,  count(1) AS amount
FROM user_logs
GROUP BY time
ORDER BY amount;
