<?php 

class UserLister extends JsonReply
{
    /** REturns a list of all users */
    function GET()
    {
        $user = $this->router()->user();
        
        if ( ! $user->mayManageUsers())
        {
            throw new NotAuthorized();
        }
        
        $users = array();
        foreach(User::listAllIds() as $id)
        {
            $user = User::loadById($id);
            $users[] = array(
                "email" => $user->email(),
                "id" => $user->id(),
                "name" => $user->name(),
                "privileges" => $user->privileges()
                );
        }
        $this->setReturnVariable("data", array("users" => $users));
    }
}



/** Returns a list of everything .*/
class UserCreator extends JsonReply
{
    public function requiresAuthentication()
    {
        return false;
    }
    
    function pseudoRandomPassword() {
        $alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890';
        $pass = array(); //remember to declare $pass as an array
        $alphaLength = strlen($alphabet) - 1; //put the length -1 in cache
        for ($i = 0; $i < 10; $i++) {
            $n = rand(0, $alphaLength);
            $pass[] = $alphabet[$n];
        }
        return implode($pass); //turn the array into a string
    }


    /** 
Creates a new user 
    
Note: `PUT`ting to this endpoint, does not require authentification.*/
    public function PUT()
    {
        $obj = $this->puttedObject();
        if( ! isset($obj->email))
        {
            throw new BadRequest();
        }
        if( ! isset($obj->name))
        {
            throw new BadRequest();
        }
        
        $u = new User();        
        $u->setEmail($obj->email);
        $u->setName($obj->name);
        $password = $this->pseudoRandomPassword();
        $u->setPassword($password);
        if( ! $u->save() )
        {
            throw new Conflict;
        }
        $this->setReturnVariable("data", array("password" => $password));

        
    }
    
}

/** Returns a list of everything .*/
class UserMaintainer extends JsonReply
{
    public function GET($email)
    {
        
        $u = User::loadByEmail($email);
        
        if ( ! $u )
        {
            throw new FileNotFound();
        }

        if ( ! $this->router()->user()->mayView($u) )
        {
            throw new NotAuthorized();            
        }
        
        
        
        $this->setReturnVariable("data", array("user" => array(
                "id" => $u->id(),
                "name" => $u->name(),
                "email" => $u->email(),
                "privileges" => $u->privileges()
                )));
    }
    
    
    /** Deletes a user  */
    public function DELETE($email)
    {
        $u = User::loadByEmail($email);
        
        if ( ! $u )
        {
            throw new FileNotFound();
        }

        if ( ! $this->router()->user()->mayDelete($u) )
        {
            throw new NotAuthorized();            
        }
        $u->delete();
    }


/**
Lists all users

NOTE: If an un authorized user tries to set the privileges of a user, the privilege part will be ignored silently. 
*/
    public function POST($email)
    {
        
        $postdata = $this->postedObject();
        if(!isset($postdata->id))
        {
            throw new BadRequest();
        }
        $user = User::loadById(intval($postdata->id));
        if ( ! $user )
        {
            throw new FileNotFound();
        }

        if ( ! $this->router()->user()->mayUpdate($user) )
        {
            throw new NotAuthorized();            
        }
        
        if(isset($postdata->name))
        {
            $user->setName($postdata->name);
        }
        if(isset($postdata->email))
        {
            $user->setEmail($postdata->email);
        }
        
        if(isset($postdata->privileges))
        {
            if (  $this->router()->user()->mayUpdatePrivileges($user) )
            {                
                $user->setPrivileges(intval($postdata->privileges));
            }            
        }

        $user->save();
    }

}
?>