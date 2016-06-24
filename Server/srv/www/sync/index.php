<?php

error_reporting(E_ALL);
ini_set('display_errors', 1);
include ("Errors.enum.php");
include ("restFramework/Router.class.php");
include ("Database.class.php");
include ("Storable.class.php");
include ("User.class.php");
include ("UserHandlers.php");
include ("Expense.class.php");
include ("ExpenseHandlers.php");

try
{
    $host = getenv("SQL_DB_IP");
    if($host == false)
    {
        throw new InternalServerError("No ip address or hostname specified for sql server.");
    }
    $user = getenv("SQL_DB_USER");
    if($user == false)
    {
        $user = "webserver";
    }
    $password = getenv("SQL_DB_PWD");
    if($password == false)
    {
        $password = "BrBa";
    }
    $database = getenv("SQL_DB_DB");
    if($database == false)
    {
        $database = "data";
    }
    $port = getenv("SQL_DB_PORT");
    if($port == false)
    {
        $port = 3306;
    }

    Database::setConnectionDetails($host, $user, $password, $database, intval($port));

    $r = new Router();
    
    $r->setAuthenticator(function() {
        if ( ! isset($_SERVER['PHP_AUTH_USER'])) {
            header('WWW-Authenticate: Basic realm="My Realm"');
            throw new NotAuthorized(Error::CredentialsNeeded);
        } 
        else 
        {
            $u = User::loadByEmail($_SERVER['PHP_AUTH_USER']);
            if( $u !== false)
            {
                if($u->matchesPassword($_SERVER['PHP_AUTH_PW'])) 
                {
                    return $u;
                }                
            }
            header('WWW-Authenticate: Basic realm="My Realm"');
            throw new NotAuthorized(Error::CredentialsMismatch);

        }
    });
    $r->setApiDocEnabled(true);

    $r->addRoute("/users", new UserCreator());
    $r->addRoute("/users", new UserLister());
    $r->addRoute("/users/{EMAIL}", new UserMaintainer());
    
    $r->addRoute("/expenses", new ExpenseCreator());
    $r->addRoute("/expenses", new ExpenseLister());
    $r->addRoute("/expenses/{N}", new ExpenseMantainer());
   
    $r->handle($_SERVER["SCRIPT_URL"]);
    
}
catch(InternalServerError $ise)
{
    $ise->sendAndDie();
}
catch(HttpException $e)
{
    $e->sendAndDie();
}
?>