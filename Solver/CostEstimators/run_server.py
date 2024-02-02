
import http.server as SimpleHTTPServer
from argparse import ArgumentParser

from models.shallow_neural_network import SNN
from models.figliozi_continuous_formula import FIG
from models.kwon_neural_network import KSNN
from models.bd_formula import BD


import socketserver
import logging
import json 
import cgi

PORT = 8000
solution_evaluator=None

parameter_to_model = {
    "SNN":  (lambda instance_name: SNN(instance_name, load_predicting_mode=True)),
    "FIG":  (lambda instance_name: FIG(instance_name, load_predicting_mode=True)),
    "BD":   (lambda instance_name: BD(instance_name, load_predicting_mode=True))
}

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
        
        costs_list = costs.tolist() if not isinstance(costs, list) else costs
        formatted_costs = ";".join(map(str, costs_list))
        
        self._set_headers()
        self.wfile.write(bytes(formatted_costs, 'utf8'))

    def do_POST(self):
        
        length = int(self.headers.get('content-length'))
        raw_content = self.rfile.read(length)
        
        instanceFile = raw_content.decode('utf8').replace("'", '"').replace("=","").split(".")[0]

        if (instanceFile is None ):
            self.send_response(400)
            self.end_headers()
            return

        
        global solution_evaluator
        solution_evaluator = parameter_to_model[args.model_type](instanceFile)
        solution_evaluator.load_model()
                
        self._set_headers()
        self.wfile.write(bytes(json.dumps({'message': 'Model loaded'}), 'utf-8'))


parser = ArgumentParser()
parser.add_argument("-p", "--port", dest="port", default=PORT)
parser.add_argument("-model_type", dest="model_type")

args = parser.parse_args()

Handler = GetHandler
httpd = socketserver.TCPServer(("", int(args.port)), Handler)

try:
    print("Listening on Port: " + str(args.port))
    httpd.serve_forever()
except KeyboardInterrupt:
    httpd.server_close()
    pass

httpd.server_close()
