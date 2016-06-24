<?php

error_reporting(E_ALL);
ini_set('display_errors', 1);

include ("restFramework/Router.class.php");


/** Gets, deletes or updates books in the database.*/
class GetUpdateDeleteBook extends JsonReply
{

/** Returns the book with ISBN number isbn as a JSON encoded object.

isbn
:    The isbn numberof the book to retreive.

*/
    public function GET($isbn)
    {
        $this->setReturnVariable("message", "I return the book with id ".$isbn);
    }
    
    
/** Updates the book with ISBN number isbn to the new data.
Expects body to be a json encoded book.

isbn
:    The ISBN number to update.*/  
    public function PUT($isbn)
    {
        $d = "";
        $this->loadPUTDataTo($d);
        $this->setReturnVariable("message", "I updates the book with id ".$isbn);
    }
    
    
    /** 
Deletes the book with id isbn from the database.

isbn
:   The ISBN number of the book to delete.

*/public function DELETE($isbn)
    {
        $this->setReturnVariable("message", "I delete the book with id $isbn");
    }
}


/** Creates a new book or lists all book ss in the database.*/
class PutListBooks extends JsonReply
{

    /** Returns a list with isbn and title of all books in the database. */
    public function GET()
    {
        $this->setReturnVariable("message", "I return a list of books");
    }
    
    
/** Creates a new book in the database and returns the isbn number. 

The data to put into the database are expected to be json encoded in the body.
*/    
    public function POST()
    {
        $d="";
        $this->setReturnVariable("message", "I create a new book the book with id ");
    }
    
}


try
{
    $r = new Router();
    
    $r->setAuthenticator(function() {
        if ( ! isset($_SERVER['PHP_AUTH_USER'])) {
            header('WWW-Authenticate: Basic realm="My Realm"');
            throw new NotAuthorized("You need to specify credentials using Basic Http Auth.");
        } 
        else 
        {
            if( $_SERVER['PHP_AUTH_USER'] !=  $_SERVER['PHP_AUTH_PW']) 
            {
                header('WWW-Authenticate: Basic realm="My Realm"');
                throw new NotAuthorized("Credentials mismatch");
            }
        }
    });
    $r->setApiDocEnabled(true);

    $r->addRoute("/books/{N:isbn}", new GetUpdateDeleteBook ());
    $r->addRoute("/books", new PutListBooks ());
    
    $r->handle($_SERVER["SCRIPT_URL"]);
    
}
catch(HttpException $e)
{
    $e->sendAndDie();
}
?>