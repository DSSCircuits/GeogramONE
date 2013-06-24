<?php 
/******************************************************************************
 * GeoGramOne TO Google Maps
 * Author:  Steven Smethurst (funvill) 
 * Email:   GeoGramOne@funvill.com 
 * Twitter: funvill 
 * 
 * This script will recive pings (HTTP Posts) from the GeoGramOne hardware and 
 * store the values in to a MySQL table. If no values are set a google map of 
 * the last ~500 or so points will be displayed. 
 *
 * More information about this script can be found on my website
 * ToDo: Insert URL to abluestar.com post. 
 *
 * Note: 
 * I made this script for my own use and I am sharing it with you for free. I 
 * would love to see where it is getting used. Please drop me a Email/Tweet 
 * just to say hi, I like meeting new people.
 *
 * MySQL table
 * ----------------------------------------------------------------------------
 * CREATE TABLE IF NOT EXISTS `gps_map` (
 *     `id` int(11) NOT NULL AUTO_INCREMENT,
 *     `modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
 *     `device_id` varchar(255) NOT NULL,
 *     `longitude` varchar(255) NOT NULL,
 *     `latitude` varchar(255) NOT NULL,
 *     `altitude` int(11) NOT NULL,
 *     `satellitesUsed` int(11) NOT NULL,
 *     `speed` int(11) NOT NULL,
 *     `battery_percent` int(11) NOT NULL,
 *     `battery_voltage` varchar(255) NOT NULL,
 *     `direction` varchar(255) NOT NULL,
 *     `date` varchar(255) NOT NULL,
 *     `time` varchar(255) NOT NULL,
 *     PRIMARY KEY (`id`)
 * ) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1005 ;
 * 
 *
 * Version table 
 * ----------------------------------------------------------------------------
 * Version | Date           | Notes
 * 0.01    | Jun 20, 2013   | Created.
 * 
 * Licence
 * ----------------------------------------------------------------------------
 * This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
 * Unported License. (CC-SA v3 license.)
 * 
 * You are free:
 *      to Share — to copy, distribute and transmit the work
 *      to Remix — to adapt the work
 *      to make commercial use of the work
 * 
 * Under the following conditions:
 *      Attribution — You must attribute the work in the manner specified by 
 *                    the author or licensor (but not in any way that 
 *                    suggests that they endorse you or your use of the work).
 *      Share Alike — If you alter, transform, or build upon this work, you 
 *                    may distribute the resulting work only under the same 
 *                    or similar license to this one.
 *
 * Please read more about this licence here 
 *      http://creativecommons.org/licenses/by-sa/3.0/
 * 
 *****************************************************************************/

 require_once('settings.php');
 
// Check to see if we want to log all requests to file 
if( DEBUG_LOG_REQUEST ) {    
    // Get the useful information. 
    $logtofile  = 'REQUEST_TIME='. $_SERVER['REQUEST_TIME'] ."\n"; 
    $logtofile .= 'REQUEST_URI='.  $_SERVER['REQUEST_URI']  ."\n";     
    $logtofile .= print_r($_REQUEST, TRUE);

    // Dump to a file 
    $fp = fopen( 'request.txt' , 'a+');
    fwrite($fp, $logtofile);
    fclose($fp);
} 
  
/**
 * Google maps supports Decimal Degree for its GPS cords. The GeoGramOne generates 
 * Degrees and decimal minutes GPS cords. We need to convert the GPS cords before 
 * using them in google maps 
 *
 * http://support.google.com/maps/bin/answer.py?hl=en&answer=2533464
 * http://stackoverflow.com/questions/2548943/gps-format-in-php/2548996#2548996
 */
function DMStoDEC($deg,$min, $sec) {
    // Converts DMS ( Degrees / minutes / seconds ) to decimal format longitude / latitude
    return $deg+((float)(( (float)$min*60)+($sec))/3600); 
}
function GeoGramStringToDEC( $GeoGramString ) {
    list($deg, $min) = sscanf($GeoGramString, "%d %f");
    if( $deg < 0 ) {
        $min *= (-1) ; 
    }
    return DMStoDEC($deg, $min, 0) ; 
}

// Check to see if this is a ping (HTTP request with data) 
if( isset($_REQUEST['act']) ) {
    if( strcmp( $_REQUEST['act'], 'ping' ) == 0 ) {
        // We want to save some data. 
        // Do we have the required prameters? 
        if( isset( $_REQUEST['id'] ) && isset( $_REQUEST['lon'] ) && isset( $_REQUEST['lat'] ) ) 
        {
            // We have everything we needed. 
            // Grab the optional prameters as well. 
            // http://php.net/manual/en/function.mysql-real-escape-string.php 
            $device_id      	= mysql_escape_string( $_REQUEST['id']    ); 
            $latitude       	= mysql_escape_string( $_REQUEST['lat']   ); 
            $longitude      	= mysql_escape_string( $_REQUEST['lon']   ); 
            $altitude       	= mysql_escape_string( $_REQUEST['alt']   ); 
            $satellitesUsed 	= mysql_escape_string( $_REQUEST['sat']   ); 
            $speed          	= mysql_escape_string( $_REQUEST['speed'] ); 
            $battery_percent	= mysql_escape_string( $_REQUEST['batp']  ); 
            $battery_voltage	= mysql_escape_string( $_REQUEST['batv']  ); 
            $direction          = mysql_escape_string( $_REQUEST['dir']   ); 
            $date          	    = mysql_escape_string( $_REQUEST['date']  ); 
            $time          	    = mysql_escape_string( $_REQUEST['time']  );          
            
            // Build the query. 
            $sqlQuery  = 'INSERT INTO '. SETTING_DATABASE_TABLE .' (`device_id` ,`longitude` ,`latitude` ,`altitude` ,`satellitesUsed` ,`speed`, `battery_percent`, `battery_voltage`, `direction`, `date`, `time` ) VALUES ("'. $device_id .'",  "'. $longitude .'",  "'. $latitude .'",  "'. $altitude .'",  "'. $satellitesUsed .'",  "'. $speed .'",  "'. $battery_percent .'",  "'. $battery_voltage .'",  "'. $direction .'",  "'. $date .'",  "'. $time .'" );';
    
            $db = mysql_connect(SETTING_DATABASE_HOST,SETTING_DATABASE_USERNAME,SETTING_DATABASE_PASSWORD) or die ('I cannot connect to the database because: ' . mysql_error());
            mysql_select_db( SETTING_DATABASE_NAME, $db );    
    
            // Run the query 
            mysql_query( $sqlQuery ) ;
            if ( mysql_errno() == 0 ) {
                // Everything looks good. 
                mysql_close($db); 	
                echo "OK"; 
                exit(); 
            }           
            
            // There was a MySQL error 
            echo "MySql Error: ". mysql_error()."\n";   
            echo "Sql Statment: ". $sqlQuery ."\n";
        } else {
            echo "Error: Missing required prameters\n"; 
        }
    } else {
        echo "Error: Unknown 'act'=[". $_REQUEST['act'] ."]\n";
    }
    
    // Something went wrong 
    echo "FAIL\n"; 
    exit(); 
}

// Output the google map of the last few recoreded points. 
?><!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no">
    <meta charset="utf-8">
    <title>GeoGramOne</title>
    <meta name="generator" content="abluestar.com" />
    <style type="text/css">
      html { height: 100% }
      body { height: 100%; margin: 0; padding: 0 }
      #map-canvas { height: 100% }
    </style>    
    <script type="text/javascript"
      src="https://maps.googleapis.com/maps/api/js?key=<?php echo SETTING_GOOGLE_API_KEY ; ?>&sensor=false">
    </script>
    <script>
function initialize() {
<?php 
    // Create a big list of avaliable points. 
    $db = mysql_connect(SETTING_DATABASE_HOST,SETTING_DATABASE_USERNAME,SETTING_DATABASE_PASSWORD) or die ('I cannot connect to the database because: ' . mysql_error());
    mysql_select_db( SETTING_DATABASE_NAME, $db );
    
    // Get the values from the database. 
    $sqlQuery = 'SELECT * FROM '.SETTING_DATABASE_TABLE.' ';
    if( isset( $_REQUEST['id'] ) ) {
        $sqlQuery .= 'WHERE device_id="'. mysql_escape_string( $_REQUEST['id'] ) .'" ';
    }
    $sqlQuery .= ' ORDER BY id DESC LIMIT 0 , 500'; // 
        
    $result = mysql_query( $sqlQuery ) ;
    if( $result == FALSE || mysql_errno() != 0 ) {
        // There was a MySQL error 
        echo "MySql Error: ". mysql_error()."\n";   
        echo "Sql Statment: ". $sqlQuery ."\n";
        exit(); 
    }
    
    if( mysql_num_rows( $result ) <= 0 ) {
        // No data. 
        echo "Error: No data was returned. \n";
        exit();         
    }
    
    echo "var locations = [";    
    $first = true ; 
    $last_latitude  = 0 ; 
    $last_longitude = 0 ; 
    
    while($row = mysql_fetch_assoc($result)) {
        if( strlen( $row['latitude']) <= 0 || strlen( $row['longitude']) <= 0 ) {
            // This is an invalid row. Skip it. 
            continue; 
        }
        
        // Check to see if this recored and the recored before it are the same. 
        if( $last_latitude  == $row['latitude'] && 
            $last_longitude == $row['longitude'] )
        {
            // This is the same cords as the last location. 
            // Lets skip it and move on 
            // ToDo: this should be handled by the SQL query. 
            continue; 
        }
        $last_latitude  = $row['latitude'] ;
        $last_longitude = $row['longitude'] ;
    
        if( $first == false ) {
            echo ",\n";
        }
        $first = false;
        
        // Generate the info box. 
        $info  = "<strong>#". $row['id']. '</strong><br />';
        $info .= "<strong>Updated</strong>: ".          $row['modified']. "<br />";
        $info .= "<strong>Car ID</strong>: ".           $row['device_id']. "<br />";
        $info .= "<strong>Battery</strong>: ".          $row['battery_percent']. "%<br />";
        $info .= "<strong>Altitude</strong>: ".         $row['altitude']. "<br />";
        $info .= "<strong>Speed</strong>: ".            $row['speed']. "<br />";
        $info .= "<strong>Satellites Used</strong>: ".  $row['satellitesUsed']. "<br />";
        $info .= "<strong>latitude</strong>: ".         $row['latitude']. "<br />";
        $info .= "<strong>longitude</strong>: ".        $row['longitude']. "<br />";
        
        // Print the line for the javascript. 
        echo "[\"". $info ."\", ". GeoGramStringToDEC( $row['latitude'] ) .", ". GeoGramStringToDEC( $row['longitude'] ) ."]";
	}
    echo "];";
    
    // We no longer need the database. Close it. 
    // mysql_close( $db );
?> 
    // Center on the last point used. 
    var myLatLng = new google.maps.LatLng(locations[0][1], locations[0][2]);
    var mapOptions = {
        zoom: 15,
        center: myLatLng,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };

    // Generate the map. 
    var map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);

    // Add the pointers to the map. 
    var infowindow = new google.maps.InfoWindow();
    var marker, i;

    // Add the current marker 
    // Car icon: Katya Sotnikova, from The Noun Project
    // http://thenounproject.com/noun/car/?dwn=PD&dwn_icon=654#icon-No12590
    marker = new google.maps.Marker({
        position: new google.maps.LatLng(locations[0][1], locations[0][2]),
        icon: "http://www.abluestar.com/temp/gps/car.png",
        map: map
    });

    google.maps.event.addListener(marker, 'click', (function(marker, i) {
        return function() {
            infowindow.setContent(locations[0][0]);
            infowindow.open(map, marker);
        }
    })(marker, 0));     
    
    
    // Do not add a pointer for the first item. it will have its own icon. 
    for (i = 1; i < locations.length; i++) {  
        marker = new google.maps.Marker({
            position: new google.maps.LatLng(locations[i][1], locations[i][2]),
            // icon: "http://labs.google.com/ridefinder/images/mm_20_blue.png",
            map: map
        });

        google.maps.event.addListener(marker, 'click', (function(marker, i) {
            return function() {
                infowindow.setContent(locations[i][0]);
                infowindow.open(map, marker);
            }
        })(marker, i));    
    }
    

    
    

    // Add the points to a poly line 
    var pathCoordinates = []; 
    for (i = 0; i < locations.length; i++) {  
        pathCoordinates.push( new google.maps.LatLng(locations[i][1], locations[i][2]) ); 
    }
    
    var routePath = new google.maps.Polyline({
        path: pathCoordinates,
        strokeColor: '#FF0000',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });

    routePath.setMap(map);
}
google.maps.event.addDomListener(window, 'load', initialize);
    </script>
  </head>
  <body>
    <?php
        // Header 
        $sqlQuery = 'SELECT DISTINCT(device_id) FROM `gps_map` ';
        
        $result = mysql_query( $sqlQuery ) ;
        if( $result == FALSE || mysql_errno() != 0 ) {
            // There was a MySQL error 
            echo "MySql Error: ". mysql_error()."\n";   
            echo "Sql Statment: ". $sqlQuery ."\n";
            exit(); 
        }
        
        if( mysql_num_rows( $result ) <= 0 ) {
            // No data. 
            echo "Error: No data was returned. \n";
            exit();         
        }
        
        echo '<div id="menu">';
        while($row = mysql_fetch_assoc($result)) {
            echo '<a href="?id='. $row['device_id'] .'">'. $row['device_id'] .'</a> | ';
        }
        echo '</div>';
    ?>
    <div id="map-canvas"></div>
  </body>
</html>
