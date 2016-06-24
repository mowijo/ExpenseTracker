<?php 


require_once("../../srv/www/sync/restFramework/Exceptions.class.php");
require_once("../../srv/www/sync/Database.class.php");

function e($cmd)
{
    //echo ">>> $cmd \n";
    $r = shell_exec($cmd);
    return $r;
}

class SqlUsingTestCase extends PHPUnit_Framework_TestCase
{

    public function testStuff(){}

    public function sqlHost()
    {   
        return "127.0.0.1";
    }

    public function sqlUser()
    {   
        return "testrunner";
    }

    public function sqlPassword()
    {   
        return "testsecret";
    }
    
    public function sqlDatabase()
    {   
        return "testing";
    }    
    
    public function sqlPort()
    {
        return 3306;
    }

    public function tearDown()
    {
    }
    
    
    public function executeSqlFile($filename)
    {
        $f = fopen($filename, "r");
        $c = "";
        while($l = fgets($f))
        {
            $l = trim($l);
            if($l == "") continue;
            $c = $c . " " . $l;
        }
        $queries = explode(";", $c);
        $d = $this->db();
        foreach($queries as $q)
        {
            $q = trim($q);
            if($q == "") continue;
            $d->h->query($q);
        }
         
    }
    
    public function setUp()
    {
        $this->executeSqlFile(dirname(__FILE__)."/../../sql/schema.sql");
        $this->executeSqlFile(dirname(__FILE__)."/../../sql/devbox-setup.sql");        
    }
    
    public function db()
    {
        Database::setConnectionDetails("127.0.0.1", "testrunner", "testsecret", "testing", 3306);
        return new Database();
    }

}


?>
