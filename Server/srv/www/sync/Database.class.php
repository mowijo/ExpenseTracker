<?php 


class DatabaseException extends InternalServerError
{
    private $m_uniqueNessViolated = false;

    public function __construct($message)
    {
        Parent::__construct(500, "Database Error: ".$message);    
    }
    
    public function markAsUniquenessViolated()
    {
        $this->m_uniqueNessViolated = true;;
    }
    
    public function violatesUniqueness()
    {
        return $this->m_uniqueNessViolated;
    }
        
}



class Database
{

    private $transactionOpen = false;
    public $h;
    
    private static $m_host;
    private static $m_user;
    private static $m_password;
    private static $m_database;
    private static $m_port;
    
    
    public static function setConnectionDetails(
        $host,
        $user,
        $password,
        $database,
        $port)
    {
        Database::$m_host = $host;
        Database::$m_user = $user;
        Database::$m_password = $password;
        Database::$m_database = $database;
        Database::$m_port= $port;
        
        
        
        
    }
    
    public function __construct()
    {/*
        if($host == false)
        {
            $host = getenv("SQL_DB_IP");
            if($host == false)
            {
                throw new InternalServerError("No ip address or hostname specified for sql server.");
            }
        }*/
        $this->h = new mysqli(Database::$m_host, Database::$m_user, Database::$m_password, Database::$m_database);
        if ($this->h->connect_errno) 
        {
            throw new DatabaseException("Failed to connect to Sql Server: (" . $this->h->connect_errno . ") " . $this->h->connect_error);
        }
    }
    
    public function __destruct()
    {
        $this->abortTransaction();
    }
    
    public function startTransaction()
    {
        if ( ! $this->transactionOpen )
        {
            $this->transactionOpen = true;
            $this->h->begin_transaction();
        }
    }
    
    public function abortTransaction()
    {
        if ( $this->transactionOpen )
        {
            $this->transactionOpen = false;
            //Abort the transaction
        }
    }

    public function commitTransaction()
    {
        if ( $this->transactionOpen )
        {
            $this->transactionOpen = false;
            $this->h->commit();
        }
    }

    

}


?>