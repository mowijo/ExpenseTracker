<?php 
abstract class Storeable
{
    var $data = array();
    
    public function asTransportableMap()
    {
        return $this->data;
    }

    private function __stringToDateTimeObject($string)
    {
        $format = "Y-m-d H:i:s";
        $d = DateTime::createFromFormat($format, $string, new DateTimeZone('UTC'));
        //$d->setTimeZone();
        return $d;
    }   

    private function __dateTimeObjecetToString($d)
    {
        $format = "Y-m-d H:i:s";
        $d->setTimeZone(new DateTimeZone('UTC'));
        return $d->format($format);
    }   

    
    private function __getValue($name, $definition)
    {
        if( ! in_array($name, array_keys($this->data)) )
        {
            $this->__setValue($name, $definition, "");
        }
        $value = $this->data[$name];
        switch($definition)
        {
            case "i": return intval($value); break;
            case "s": return strval($value); break;
            case "ts": return $this->__stringToDateTimeObject($value); 
            default :
                throw new InternalServerError("Type $definition unknown in class Storeable");
        }
    }
    
    private function __getValueAsString($name, $definition)
    {
        $d = $this->__getValue($name, $definition);
        if($definition == "ts")
        {
            return $d = $this->__dateTimeObjecetToString($d);
        }
        return $d;
    }

    protected function __setValue($name, $definition, $value)
    {
        switch($definition)
        {
            case "i": $value = intval($value); break;
            case "s": $value = strval($value); break;
            case "ts":
                
                if ( ! is_object($value) )
                {
                    if( is_string($value) && $value == "") 
                    {
                        //This is the default value we fall back to if nothing is set.
                        $mtime = new DateTime();        
                        $mtime->setTimezone( new DateTimeZone('UTC') );
                        $mtime->setDate ( 2000, 01, 01);
                        $mtime->setTime ( 00, 00, 01);
                        $this->__setValue($name, $definition, $mtime);
                        return;

                    }
                    else
                    {
                        throw new InternalServerError("Cannot set a timestamp to non object non \'\' value '$value'");
                    }
                }
                else
                {
                    $value = $this->__dateTimeObjecetToString($value); 
                }
                break;
                
            default :
                throw new InternalServerError("Type $definition unknown in class Storeable");
        }
        
        $this->data[$name] = $value;
    }
    
    public function sqlDefinitionToPhpType($def)
    {
        if(strtolower(substr($def, 0, 3)) == "int") return "i";    
        if(strtolower(substr($def, 0, 7)) == "varchar") return "s";    
        if(strtolower(substr($def, 0, 4)) == "char") return "s";    
        if(strtolower(substr($def, 0, 9)) == "timestamp") return "ts";    
        
        
        throw new InternalServerError("Type $def unknown in class Storeable");

    }
    protected abstract function __propertyDefinitions();
    
    public function propertyDefinitions()
    {
        $a = $this->__propertyDefinitions();
        $a["id"] = "int primary key not null auto_increment";
        return $a;
    }
    
    private function hasField($fieldname)
    {
        $defs = $this->propertyDefinitions(); 
        return in_array($fieldname, array_keys($this->propertyDefinitions()));            
    }
    
    
    private function sqlTypeToPreparedBindingCharacter($def)
    {
        if(strtolower(substr($def, 0, 3)) == "int") return "i";
        return "s";    
    }
    
    public abstract function tableName();
    
    
    private function __insert($d)
    {

        $q = "";
        $v = "";
        $bindingformat = "";
        $data = array();
        foreach($this->propertyDefinitions() as $field => $definition)
        {
            $bindingformat .= $this->sqlTypeToPreparedBindingCharacter($definition);
            
            if($q != "") $q = $q . ", ";
            if($v != "") $v = $v . ", ";
            $q .= "$field";
            $v = $v."?";
            $data[] = $this->__getValueAsString($field, $this->sqlDefinitionToPhpType($definition));

        }

        $q = "INSERT INTO ".$this->tableName()." ($q) VALUES ($v)";
        
            
        if ( ! ($stmt = $d->h->prepare($q)) )  
        {

            throw new DatabaseException("Failed to prepare statement '$q': (" . $d->h->connect_errno . ") " . $d->h->connect_error);
        }
        
        $params = array_merge(array($bindingformat), $data);
        
        if (! call_user_func_array(array($stmt, 'bind_param'), $this->makeValuesReferenced($params)) ) 
        {
            throw new DatabaseException("Failed to bind parameter ($value) for $q :  (" . $stmt->errno . ") " . $stmt->error); 
        }

        if (!$stmt->execute()) 
        {       
            $d = new DatabaseException("Failed to execute statement '$q' with ':  (" . $stmt->errno . ") " . $stmt->error);     
            if($stmt->errno == 1062)
            {
                $d->markAsUniquenessViolated();
            }
            throw $d;
        }

        
        $this->data["id"] = intval($stmt->insert_id);

        return true;
    
    }
    
    
    
    private function __update($d)
    {
        $q = "";
        $bindingformat = "";
        $data = array();
        foreach($this->propertyDefinitions() as $field => $definition)
        {
            if($field == "id") continue;
            $bindingformat .= $this->sqlTypeToPreparedBindingCharacter($definition);
            
            if($q != "") $q = $q . ", ";
            $q .= " $field = ?";
            $data[] = $this->__getValue($field, "s");
        }
        $q = "UPDATE ".$this->tableName()." SET $q WHERE id = ?";
        $bindingformat .= "i";
        $data[] = $this->data["id"];
        
        if ( ! ($stmt = $d->h->prepare($q)) )  
        {
            throw new DatabaseException("Failed to prepare statement '$q': (" . $d->h->connect_errno . ") " . $d->h->connect_error);
        }
        
        $params = array_merge(array($bindingformat), $data); 
        if (! call_user_func_array(array($stmt, 'bind_param'), $this->makeValuesReferenced($params)) ) 
        {
            throw new DatabaseException("Failed to bind parameter ($value) for $q :  (" . $stmt->errno . ") " . $stmt->error); 
        }
        if (!$stmt->execute()) 
        {       
            $d = new DatabaseException("Failed to execute statement '$q' with ':  (" . $stmt->errno . ") " . $stmt->error); 
            if($stmt->errno == 1062)
            {
                $d->markAsUniquenessViolated();
            }
            throw $d;
        }
                
        preg_match_all ('/(\S[^:]+): (\d+)/', $d->h->info, $matches); 
        $info = array_combine ($matches[1], $matches[2]);
        if(intval($info["Rows matched"]) == 0)
        {
            throw new DatabaseException("Tried to update a user with id ".$this->data["id"]." that does not exist in database."); 
        }                
        return true;
    
    }    
    
    private function makeValuesReferenced($arr){ 
        $refs = array(); 
        foreach($arr as $key => $value) 
        $refs[$key] = &$arr[$key]; 
        return $refs; 

    } 
    
    
    public function save($d = false)
    {
        if(! $d) 
        {
            $d = new Database();
        }
        try
        {
            if( $this->__getValue("id", "i") == 0)
            {
                return $this->__insert($d);
            }
            else
            {
                return $this->__update($d);
            }
        }
        catch(DatabaseException $dbe)
        {
            if($dbe->violatesUniqueness() ) 
            {
                throw new Conflict();
            }
            throw $dbe;
        }
    
    }
    
    private function loadBy($fieldname, $value)
    {   
        $d = new Database();
        if(!$this->hasField($fieldname))
        {
            throw new InternalServerError(get_class($this). " has no field named $fieldname");        
        }
        
        $q = "";
        $bindingformat = "";
        foreach($this->propertyDefinitions() as $field => $definition)
        {
            if($fieldname == $field)
            {
                $bindingformat = $this->sqlTypeToPreparedBindingCharacter($definition);
            }
            
            if($q != "") $q = $q . ", ";
            $q .= "$field";
        }
        $q = "SELECT ".$q." FROM ".$this->tableName()." WHERE $fieldname = ?";
        if ( ! ($stmt = $d->h->prepare($q)) )  
        {
            throw new DatabaseException("Failed to prepare statement '$q': (" . $d->h->connect_errno . ") " . $d->h->connect_error);
        }
        if (!$stmt->bind_param($bindingformat, $value)) 
        {
            throw new DatabaseException("Failed to bind parameter ($value) for $q :  (" . $stmt->errno . ") " . $stmt->error); 
        }
        if (!$stmt->execute()) 
        {   
            throw new DatabaseException("Failed to execute statement '$q' with parameter '$value':  (" . $stmt->errno . ") " . $stmt->error); 
        }
        
        $res = $stmt->get_result();
        if(! $res->num_rows == 1)
        {
            return false;
        }
        $this->data = $res->fetch_assoc();
        return true;
    }
    
    public function delete($db = false)
    {
        if(!$db)
        {
            $d = new Database();
        }
        else
        {
            $d = $db;
        }
        if ( ! ($stmt = $d->h->prepare("DELETE FROM ".$this->tableName()." WHERE id = ?")) )  
        {
            throw new DatabaseException("Failed to prepare statement '$q': (" . $d->h->connect_errno . ") " . $d->h->connect_error);
        }
        $id = $this->data["id"];
        if (!$stmt->bind_param("i", $id)) 
        {
            throw new DatabaseException("Failed to bind parameter ($value) for $q :  (" . $stmt->errno . ") " . $stmt->error); 
        }
        if (!$stmt->execute()) 
        {           
            throw new DatabaseException("Failed to execute statement '$q' with parameter '$value':  (" . $stmt->errno . ") " . $stmt->error); 
        }
    }
    
    
    public static function __callStatic($name, $arguments)
    {
        if(substr($name, 0, 6) != "loadBy")
        {
            throw new InternalServerError("$name is neither a dynamic nor a declared method");
        }
        $field = strtolower(substr($name, 6));
        $classname = get_called_class();
        $o = eval("return new $classname ;");
        
        if(sizeof($arguments) < 2)
        {
            $d = new Database();
        }
        else
        {
            $d = $arguments[1];
        }
        if( ! $o->loadBy($field, $arguments[0] , $d))
        {
            return false;
        }
        return $o;
    }

    
    public function __call($name, $arguments)
    {
        $is_getter = true;
        // Note: value of $name is case sensitive.
        
        if(substr($name, 0,3) == "set")
        {
            $is_getter = false;
            $name = substr($name, 3);
        }
        
        $name = lcfirst($name);
        
        $defs  = $this->propertyDefinitions();
        
        if ( in_array($name, array_keys($defs)) )
        {
            if($is_getter)
            {
                return $this->__getValue($name, $this->sqlDefinitionToPhpType($defs[$name]));
            }
            else
            {
                $this->__setValue($name, $this->sqlDefinitionToPhpType($defs[$name]), $arguments[0]);
                return;
            }
        }
        
        throw new InternalServerError("$name is neither a dynamic nor a declared method");
    }
    
    public static function listAllIds()
    {   
        $d = new Database();
        $instance = eval("return new ".get_called_class()."();");
        $res = $d->h->query("SELECT id FROM ".$instance->tableName());

        $ids = array();
        for ($row_no = $res->num_rows - 1; $row_no >= 0; $row_no--) {
            $res->data_seek($row_no);
            $row = $res->fetch_assoc();
            $ids[] = intval($row['id']);
        }
        return $ids;
                
        
    }
}


?>