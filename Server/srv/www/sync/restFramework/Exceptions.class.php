<?php 


class PrintReporter
{
    public function report($s)
    {
        echo $s;
    }
}

// 
/** HttpExceptions output JSON encoded information!*/
abstract class HttpException extends Exception
{
    private $m_reason;
    private $m_trace;
    
    private $m_errorid = -1;
    private $m_errordetails = array();

    public function __construct($errorcode, $reason = "")
    {
        parent::__construct("", intval($errorcode));
        if(is_array($reason))
        {
            $this->m_reason = $reason;
        }
        else
        {
            $this->m_reason = array();
            $reason = trim(strval($reason));
            if($reason != "")
            {
                $this->m_reason["reason"] = $reason;
            }
        }
            $this->m_reason["statuscode"] = $errorcode;
        $this->m_reason["success"] = false;

        
        ob_start();
        echo "Stack Trace:\n";
        debug_print_backtrace();
        echo "\n\nServer env:\n";
        var_dump($_SERVER);
        $this->m_trace = ob_get_contents();
        ob_end_clean();
    }
    
    public function setErrorId($id)
    {   
        $this->m_errorid = $id;
    }
    
    public function setErrorDetails($details)
    {
        if(is_array($details))
        {
            $this->m_errordetails = $details;
        }
        else
        {
            $this->m_errordetails[] = $details;
        }
    }
    
    public function reason()
    {
        return $this->m_reason;
    }
    
    public function statusCode()
    {
        return $this->getCode();
    }
    
    public function sendAndDie()
    {
        if($this->m_errorid != -1)
        {
            $this->m_reason["error"] = array("id"=>$this->m_errorid, "details"=> $this->m_errordetails);
        }        
        http_response_code($this->getCode());
        header("Content-Type: text/json");
        echo json_encode($this->m_reason);
        die();    
    }
    
    public function report($reporter)
    {
        if( ! $this->mustReportToDevOps() ) return;
        $reporter->report($this->m_trace);
    }
    
    public abstract function mustReportToDevOps();
}


class FileNotFound extends HttpException
{
    public function __construct($errorid = -1, $details = array())        
    {
        Parent::__construct(404);
        $this->setErrorId($errorid);
        $this->setErrorDetails($details);
    }
    
    public function mustReportToDevOps() { return false; }
}





class MethodNotImplementedException extends HttpException
{  
    public function __construct($errorid = -1, $errordetails = array())
    {
        Parent::__construct(501);
        $this->setErrorId($errorid);
        $this->setErrorDetails($errordetails);
    }

    public function mustReportToDevOps() { return false; }

}

class InternalServerError extends HttpException
{  
    public function __construct($message)
    {
        Parent::__construct(500, "Internal server error: ".$message);    
    }
    
    public function mustReportToDevOps() { return true; }
    
    public function sendAndDie()
    {
        http_response_code(500);
        header("Content-Type: text/json");
        echo json_encode(array("success" => false, "statuscode" => 500));
        die();    
    }

}


class BadRequest extends HttpException
{
    public function __construct($errorid = -1, $errordetails = array())
    {
        Parent::__construct(400);    
        $this->setErrorId($errorid);
        $this->setErrorDetails($errordetails);
    }
    
    public function mustReportToDevOps() { return false; }

}

class NotAuthorized extends HttpException
{
    public function __construct($errorid = -1, $errordetails = array())
    {
        Parent::__construct(401);    
        $this->setErrorId($errorid);
        $this->setErrorDetails($errordetails);
    }
    
    public function mustReportToDevOps() { return false; }
    
}

class Conflict extends HttpException
{
    public function __construct($errorid = -1, $details = array())
    {
        Parent::__construct(409);
        $this->setErrorDetails($details);
        $this->setErrorId($errorid);
    }
    
    public function mustReportToDevOps() { return false; }
    
}

class Gone extends HttpException
{
    public function __construct($errorid = -1, $details = array())
    {
        Parent::__construct(410);
        $this->setErrorDetails($details);
        $this->setErrorId($errorid);
    }
    
    public function mustReportToDevOps() { return false; }
    
}


class UpgradeRequired extends HttpException
{
    public function __construct($message = "Upgrade Required")
    {
        Parent::__construct(426, $message);    
    }
    
    public function mustReportToDevOps() { return true; }
    
}


?>