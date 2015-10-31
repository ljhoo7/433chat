<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ page contentType="text/html; charset=utf-8" pageEncoding="utf-8"%>
<!DOCTYPE html>
<html lang="en">

<head>

    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="">
    <meta name="author" content="">

    <title>Manage Server Dashboard</title>

    <!-- Bootstrap Core CSS -->
    <link href="resources/bower_components/bootstrap/dist/css/bootstrap.min.css" rel="stylesheet">

    <!-- MetisMenu CSS -->
    <link href="resources/bower_components/metisMenu/dist/metisMenu.min.css" rel="stylesheet">

    <!-- Timeline CSS -->
    <link href="resources/dist/css/timeline.css" rel="stylesheet">

    <!-- Custom CSS -->
    <link href="resources/dist/css/sb-admin-2.css" rel="stylesheet">

    <!-- Morris Charts CSS -->
    <link href="resources/bower_components/morrisjs/morris.css" rel="stylesheet">

    <!-- Custom Fonts -->
    <link href="resources/bower_components/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">

    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
        <script src="https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js"></script>
        <script src="https://oss.maxcdn.com/libs/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->

</head>

<body>

    <div id="wrapper">

        <!-- Navigation -->
        <nav class="navbar navbar-default navbar-static-top" role="navigation" style="margin-bottom: 0">
            <div class="navbar-header">
                <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
                    <span class="sr-only">Toggle navigation</span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                </button>
                <a class="navbar-brand" href="/index">Manage Server Dashboard</a>
            </div>
            <!-- /.navbar-header -->

            <ul class="nav navbar-top-links navbar-right">
                <!-- /.dropdown -->
                <li class="dropdown">
                    <a class="dropdown-toggle" data-toggle="dropdown" href="#">
                        <i class="fa fa-bell fa-fw"></i>  <i class="fa fa-caret-down"></i>
                    </a>
                    <ul class="dropdown-menu dropdown-alerts">
                        
                    </ul>
                    <!-- /.dropdown-alerts -->
                </li>
            </ul>
            <!-- /.navbar-top-links -->

            <div class="navbar-default sidebar" role="navigation">
                <div class="sidebar-nav navbar-collapse">
                    <ul class="nav" id="side-menu">
                        <li class="sidebar-search">
                            <div class="input-group custom-search-form">
                                <input type="text" class="form-control" placeholder="Search...">
                                <span class="input-group-btn">
                                <button class="btn btn-default" type="button">
                                    <i class="fa fa-search"></i>
                                </button>
                            </span>
                            </div>
                            <!-- /input-group -->
                        </li>
                        <li>
                            <a href="index"><i class="fa fa-dashboard fa-fw"></i> Dashboard</a>
                        </li>
                    </ul>
                </div>
                <!-- /.sidebar-collapse -->
            </div>
            <!-- /.navbar-static-side -->
        </nav>

        <div id="page-wrapper">
            <div class="row">
                <div class="col-lg-12">
                    <h1 class="page-header">Dashboard <a href="get" class ="btn btn-default"><i class="fa fa-refresh fa-fw"></i> Request New Agent Data</a></h1>
                    
                </div>
                <!-- /.col-lg-12 -->
            </div>
            <!-- /.row -->
            <div class="row">
                <div class="col-lg-3 col-md-6">
                    <div class="panel panel-primary">
                        <div class="panel-heading">
                            <div class="row">
                                <div class="col-xs-3">
                                    <i class="fa fa-desktop fa-5x"></i>
                                </div>
                                <div class="col-xs-9 text-right">
                                    <div class="huge">${overview[0]}</div>
                                    <div>Servers</div>
                                </div>
                            </div>
                        </div>
                        <a href="#">
                            <div class="panel-footer">
                                <span class="pull-left">View Details</span>
                                <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                                <div class="clearfix"></div>
                            </div>
                        </a>
                    </div>
                </div>
                <div class="col-lg-3 col-md-6">
                    <div class="panel panel-green">
                        <div class="panel-heading">
                            <div class="row">
                                <div class="col-xs-3">
                                    <i class="fa fa-users fa-5x"></i>
                                </div>
                                <div class="col-xs-9 text-right">
                                    <div class="huge">${overview[1]}</div>
                                    <div>Users</div>
                                </div>
                            </div>
                        </div>
                        <a href="#">
                            <div class="panel-footer">
                                <span class="pull-left">View Details</span>
                                <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                                <div class="clearfix"></div>
                            </div>
                        </a>
                    </div>
                </div>
                <div class="col-lg-3 col-md-6">
                    <div class="panel panel-yellow">
                        <div class="panel-heading">
                            <div class="row">
                                <div class="col-xs-3">
                                    <i class="fa fa-home fa-5x"></i>
                                </div>
                                <div class="col-xs-9 text-right">
                                    <div class="huge">${overview[2]}</div>
                                    <div>Rooms</div>
                                </div>
                            </div>
                        </div>
                        <a href="#">
                            <div class="panel-footer">
                                <span class="pull-left">View Details</span>
                                <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                                <div class="clearfix"></div>
                            </div>
                        </a>
                    </div>
                </div>
                <div class="col-lg-3 col-md-6">
                    <div class="panel panel-red">
                        <div class="panel-heading">
                            <div class="row">
                                <div class="col-xs-3">
                                    <i class="fa fa-link fa-5x"></i>
                                </div>
                                <div class="col-xs-9 text-right">
                                    <div class="huge">${overview[3]}</div>
                                    <div>Agents</div>
                                </div>
                            </div>
                        </div>
                        <a href="#">
                            <div class="panel-footer">
                                <span class="pull-left">View Details</span>
                                <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                                <div class="clearfix"></div>
                            </div>
                        </a>
                    </div>
                </div>
            </div>
            <!-- /.row -->
            <div class="row">
                <div class="col-lg-8">
		        <!-- group -->
            	<div class="panel panel-green">
                    <div class="panel-heading">
                       <i class="fa fa-list-alt fa-fw"></i> Room Info
                    </div>
                    <!-- /.panel-heading -->
                    <div class="panel-body">
                   	[Server List]<br>
	                   	<div class="panel panel-default" style="padding:10px;margin-bottom:5px;">
	                   		
	                   		<c:if test="${overview[0] ne 0}">
		                    	<c:forEach var="i" begin="0" end="${overview[0]-1}">
			                   		<a href="" class="btn btn-outline btn-info">${allServer[i]}</a>
		                   		</c:forEach>
		                	</c:if>
	            		</div>

	                   	[Room List]<br>
	                   	<div class="panel panel-default" style="padding:10px">
	                   		<c:if test="${overview[2] ne 0}">
		                    	<c:forEach var="i" begin="0" end="${overview[2]-1}">
				                   	<div class="wall">room_name : ${allRoomname[i]}</div>
				                   	<c:if test="${allUser[i][0] ne 0}">
			                    		<c:forEach var="j" begin="1" end="${allUser[i][0]}">
						                   	<a href="" class="btn btn-outline btn-primary">${allUser[i][j]}</a>
					                   	</c:forEach>
				                   	</c:if>
				                   	<a href="destroy/${allRoomname[i]}" class="btn btn-danger">Destroy</a>
			                   		<hr>
		                   		</c:forEach>
		                   	</c:if>
	                    </div>
	           		</div>                     
            	</div>            	
		        <!-- /#server -->
                </div>
                <!-- /.col-lg-8 -->
                <div class="col-lg-4">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <i class="fa fa-bell fa-fw"></i> Notifications Panel
                        </div>
                        <!-- /.panel-heading -->
                        <div class="panel-body">
                            <div class="list-group">
                            
				      				<c:forEach var="i" begin="1" end="${notify[0][0]}">                        
		                                <a href="#" class="list-group-item">
		                                    <i class="fa fa-warning fa-fw"></i> ${notify[i][1]}
		                                    <span class="pull-right text-muted small"><em>${notify[i][2]}</em>
		                                    </span>
		                                </a>
	                                </c:forEach>
                                
                            </div>
                            <!-- /.list-group -->
                            <a href="#" class="btn btn-default btn-block">View All Alerts</a>
                        </div>
                        <!-- /.panel-body -->
                    </div>
                    <!-- /.panel -->
                </div>
                <!-- /.col-lg-4 -->            
            <!-- /.row -->
        	</div>
			<div class="row">
			
			<div class="col-lg-12">
		        <!-- server -->
		        <c:if test="${overview[3] ne 0}">
			        <c:forEach var="i" begin="0" end="${overview[3]-1}">
	            	<div class="panel panel-primary">
	                    <div class="panel-heading">
	                       <i class="fa fa-desktop fa-fw"></i>${agentName[i]}
	                    </div>
	                    <!-- /.panel-heading -->
	                    <div class="panel-body">
	                   		[Server List]<br>
		                   	<div class="panel panel-default" style="padding:10px;">
		                   		<c:if test="${serverList[i][0] ne 0}">
			                    	<c:forEach var="j" begin="1" end="${serverList[i][0]}">
			                   		<a href="" class="btn btn-outline btn-info">${serverList[i][j]}</a>
			                   		<a href="kill/${agentName[i]}/${serverList[i][j]}" class="btn btn-danger btn-circle"><i class="fa fa-times fa-fw"></i></a>
									</c:forEach>
								</c:if>
								<c:if test="${genState eq 0}">
									<a href="generate/${agentName[i]}" class="btn btn-default">Generate</a>
								</c:if>
								<c:if test="${genState eq 1}">
									<a href="generate/${agentName[i]}" class="btn btn-default disabled">Generating...</a>
								</c:if>
		                   	</div>
		
		                   	[User List]<br>
		                   	<div class="panel panel-default" style="padding:10px">
		                   		<c:if test="${userList[i][0] ne 0}">
			                   		<c:forEach var="j" begin="1" end="${userList[i][0]}">
				                   		<a href="" class="btn btn-outline btn-primary">${clientServer[i][j]}_${userList[i][j]}(${clientSocket[i][j]})</a>
					                   	<a href="out/${clientServer[i][j]}/${clientSocket[i][j]}" class="btn btn-danger btn-circle" ><i class="fa fa-times fa-fw"></i></a>
				                   	</c:forEach>
				                </c:if>
		                   	</div>
	                    </div>
	                    <div class="panel-footer">
	           			</div>                        
	            	</div>            	
					
					</c:forEach>
				</c:if>
		        <!-- /#server -->
			</div>
        <!-- /#page-wrapper -->
		</div>
    	</div>
   	</div>
    <!-- /#wrapper -->

    <!-- jQuery -->
    <script src="resources/bower_components/jquery/dist/jquery.min.js"></script>

    <!-- Bootstrap Core JavaScript -->
    <script src="resources/bower_components/bootstrap/dist/js/bootstrap.min.js"></script>

    <!-- Metis Menu Plugin JavaScript -->
    <script src="resources/bower_components/metisMenu/dist/metisMenu.min.js"></script>

    <!-- Custom Theme JavaScript -->
    <script src="resources/dist/js/sb-admin-2.js"></script>

</body>

</html>
