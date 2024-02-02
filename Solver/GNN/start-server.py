
import http.server as SimpleHTTPServer
from solution_evaluator import SolutionEvaluator
from argparse import ArgumentParser
import socketserver
import logging
import json 
import cgi

PORT = 8000
solution_evaluator=None

class GetHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):

    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def do_PUT(self):
        length = int(self.headers.get('content-length'))
        content = self.rfile.read(length).decode('utf8').replace("'", '"')
        
        solutionArray = [[int(block) for block in blocks.split(',') if block.isdigit()] for blocks in content.split(';')][:-1]

        if (solutionArray is None):
            self.send_response(400)
            self.end_headers()
            return

        global solution_evaluator
        costs = solution_evaluator.evaluate(solutionArray)

        self._set_headers()
        self.wfile.write(bytes(costs, 'utf8'))

    def do_POST(self):
        
        length = int(self.headers.get('content-length'))
        raw_content = self.rfile.read(length)
        
        instanceFile = raw_content.decode('utf8').replace("'", '"').replace("=","")

        if (instanceFile is None ):
            self.send_response(400)
            self.end_headers()
            return

        global solution_evaluator
        solution_evaluator = SolutionEvaluator(instance_file=instanceFile)
        
        self._set_headers()
        self.wfile.write(bytes(json.dumps({'message': 'Model loaded'}), 'utf-8'))



parser = ArgumentParser()
parser.add_argument("-p", "--port", dest="port", default=PORT)
args = parser.parse_args()

Handler = GetHandler
httpd = socketserver.TCPServer(("", int(args.port)), Handler)

try:
    print("Listening on Port: " + args.port)
    httpd.serve_forever()
except KeyboardInterrupt:
    httpd.server_close()
    pass

httpd.server_close()
